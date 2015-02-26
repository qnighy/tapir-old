#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstdint>
#include <map>
#include <string>

#include <SDL.h>
#include <ruby.h>

#include "file_misc.h"

const char *game_path = "/home/qnighy/workdir/vxace/SakusakuEnc";
const char *rtp_path = "/home/qnighy/workdir/vxace/RTP/RPGVXAce";

struct ArchiveEntry {
  uint32_t key;
  uint32_t entry_pos;
  uint32_t entry_size;
  uint32_t iopos;
};

static SDL_RWops *archive;
static uint32_t archive_key;
static std::map<std::string, ArchiveEntry> archive_entries;

static VALUE rb_load_data(VALUE self, VALUE path);

void InitFileMisc() {
  chdir(game_path);
  archive = SDL_RWFromFile("Game.rgss3a", "rb");
  if(archive) {
    char header[8];
    SDL_RWread(archive, header, 1, 8);
    if(memcmp(header, "RGSSAD\x00\x03", 8)) {
      goto failure;
    }
    archive_key = SDL_ReadLE32(archive) * 9 + 3;
    fprintf(stderr, "archive_key = 0x%08x\n", archive_key);
    for(;;) {
      ArchiveEntry entry;
      entry.entry_pos = SDL_ReadLE32(archive) ^ archive_key;
      entry.entry_size = SDL_ReadLE32(archive) ^ archive_key;
      entry.key = SDL_ReadLE32(archive) ^ archive_key;
      entry.iopos = 0;
      uint32_t filename_size = SDL_ReadLE32(archive) ^ archive_key;
      if(entry.entry_pos == 0) break;
      std::string filename;
      filename.resize(filename_size);
      SDL_RWread(archive, &filename[0], 1, filename_size);
      for(uint32_t i = 0; i < filename_size; ++i) {
        filename[i] ^= archive_key>>((i&3)<<3);
      }
      for(uint32_t i = 0; i < filename_size; ++i) {
        if(filename[i] == '\\') {
          filename[i] = '/';
        }
      }
      archive_entries[filename] = entry;
    }
  } else {
failure:
    archive = nullptr;
  }

  rb_define_method(rb_mKernel, "load_data",
      (VALUE(*)(...))rb_load_data, 1);
}

static Sint64 archive_entry_size(SDL_RWops *rwops) {
  ArchiveEntry *entry = (ArchiveEntry*)rwops->hidden.unknown.data1;
  return entry->entry_size;
}
static Sint64 archive_entry_seek(
    SDL_RWops *rwops, Sint64 offset, int whence) {
  ArchiveEntry *entry = (ArchiveEntry*)rwops->hidden.unknown.data1;
  uint32_t newpos;
  if(whence == RW_SEEK_SET) {
    newpos = offset;
  } else if(whence == RW_SEEK_CUR) {
    newpos = entry->iopos + offset;
  } else if(whence == RW_SEEK_END) {
    newpos = entry->entry_size + offset;
  } else {
    return SDL_SetError("Invalid whence field in seek");
  }
  if(!(0 <= newpos && newpos <= entry->entry_size)) {
    return SDL_SetError("seek position out-of-range");
  }
  while(newpos > entry->iopos) {
    if((entry->iopos&3)==3) {
      entry->key = entry->key * 7u + 3u;
    }
    ++entry->iopos;
  }
  while(newpos < entry->iopos) {
    if((entry->iopos&3)==0) {
      entry->key = (entry->key - 3u) * 3067833783u;
    }
    --entry->iopos;
  }
  return newpos;
}
static size_t archive_entry_read(
    SDL_RWops *rwops, void *ptr, size_t size, size_t num) {
  ArchiveEntry *entry = (ArchiveEntry*)rwops->hidden.unknown.data1;
  if(size * num > entry->entry_size-entry->iopos) {
    num = (entry->entry_size-entry->iopos)/size;
  }
  SDL_RWseek(archive, entry->entry_pos + entry->iopos, RW_SEEK_SET);
  num = SDL_RWread(archive, ptr, size, num);
  for(size_t i = 0; i < size*num; ++i) {
    size_t x = entry->iopos+i;
    ((char*)ptr)[i] ^= entry->key>>((x&3)<<3);
    if((x&3)==3) {
      entry->key = entry->key * 7u + 3u;
    }
  }
  entry->iopos += size * num;
  return num;
}
static size_t archive_entry_write(SDL_RWops *, const void *, size_t, size_t) {
  return SDL_SetError("Can't write RGSSAD3 entry");
}
static int archive_entry_close(SDL_RWops *rwops) {
  ArchiveEntry *entry = (ArchiveEntry*)rwops->hidden.unknown.data1;
  delete entry;
  SDL_FreeRW(rwops);
  return 0;
}

static SDL_RWops *openFileCaseInsensitiveTweak(
    std::string base, std::string path);

SDL_RWops *openPath(std::string path) {
  for(char &c : path) {
    if(c == '\\') {
      c = '/';
    }
  }
  auto iter = archive_entries.find(path);
  if(iter != archive_entries.end()) {
    ArchiveEntry *entry = new ArchiveEntry;
    *entry = iter->second;
    SDL_RWops *ret = SDL_AllocRW();
    if(!ret) return nullptr;
    ret->size = archive_entry_size;
    ret->seek = archive_entry_seek;
    ret->read = archive_entry_read;
    ret->write = archive_entry_write;
    ret->close = archive_entry_close;
    ret->type = 0x43322113;
    ret->hidden.unknown.data1 = entry;
    return ret;
  }
  SDL_RWops *rwops = SDL_RWFromFile(path.c_str(), "rb");
  if(rwops) return rwops;
  rwops = SDL_RWFromFile((rtp_path+("/"+path)).c_str(), "rb");
  if(rwops) return rwops;
  rwops = openFileCaseInsensitiveTweak(".", path);
  if(rwops) return rwops;
  rwops = openFileCaseInsensitiveTweak(rtp_path, path);
  if(rwops) return rwops;
  return nullptr;
}

static VALUE rb_load_data(VALUE, VALUE path) {
  SDL_RWops *rwops = openPath(StringValueCStr(path));
  if(!rwops) return Qnil;

  long len= SDL_RWsize(rwops);
  char *ptr = new char[len];
  SDL_RWread(rwops, ptr, 1, len);
  SDL_RWclose(rwops);
  VALUE str = rb_str_new(ptr, len);
  delete[] ptr;

  VALUE rb_mMarshal =
    rb_funcall(rb_cObject,
        rb_intern("const_get"), 1,
        ID2SYM(rb_intern("Marshal")));
  return rb_funcall(rb_mMarshal, rb_intern("load"), 1, str);
}

static char lower(char ch) {
  if('A' <= ch && ch <= 'Z') return ch - 'A' + 'a';
  return ch;
}

static SDL_RWops *openFileCaseInsensitiveTweak(
    std::string base, std::string path) {
  int pathpos = 0;
  while((size_t)pathpos < path.size()) {
    int nextpos = path.find('/', pathpos);
    if(nextpos == -1) nextpos = path.size();
    std::string pathelem(path, pathpos, nextpos-pathpos);
    DIR *dir = opendir(base.c_str());
    if(!dir) return nullptr;
    bool found = false;
    dirent *dent;
    while((dent = readdir(dir)) != nullptr) {
      bool eql = true;
      int i;
      for(i = 0; dent->d_name[i]; ++i) {
        if((size_t)i >= pathelem.size()) {
          eql = false;
          break;
        }
        if(lower(dent->d_name[i]) != lower(pathelem[i])) {
          eql = false;
          break;
        }
      }
      if(pathelem.size() != (size_t)i) eql = false;
      if(eql) {
        pathelem = dent->d_name;
        found = true;
        break;
      }
    }
    closedir(dir);
    if(!found) return nullptr;
    base.push_back('/');
    base.append(pathelem);
    pathpos = nextpos + 1;
  }
  SDL_RWops *rwops = SDL_RWFromFile(base.c_str(), "rb");
  return rwops;
}
