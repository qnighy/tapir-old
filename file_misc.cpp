#include <unistd.h>
#include <cstdint>
#include <map>
#include <string>

#include <SDL.h>
#include <ruby.h>

#include "file_misc.h"

const char *game_path = "/home/qnighy/workdir/vxace/SakusakuEnc/";
const char *rtp_path = "/home/qnighy/workdir/vxace/RTP/RPGVXAce/";

struct ArchiveEntry {
  uint32_t key;
  uint32_t entry_pos;
  uint32_t entry_size;
  uint32_t iopos;
};

static SDL_RWops *archive;
static uint32_t archive_key;
std::map<std::string, ArchiveEntry> archive_entries;

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
      fprintf(stderr, "key = 0x%08x, pos = %u, size = %u, filename_size = %u\n",
          entry.key, entry.entry_pos, entry.entry_size, filename_size);
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
      fprintf(stderr, "filename = %s\n", filename.c_str());
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
static Sint64 archive_entry_seek(SDL_RWops *, Sint64, int) {
  return SDL_SetError("Can't seek RGSSAD3 entry");
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
  return nullptr;
}

static VALUE rb_load_data(VALUE self, VALUE path) {
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
