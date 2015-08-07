#ifndef FILE_MISC_H_INCLUDED
#define FILE_MISC_H_INCLUDED

#include <string>
#include <SDL.h>

// extern const char *game_path;
extern const char *rtp_path;

void InitFileMisc();
SDL_RWops *openPath(std::string path);


#endif // FILE_MISC_H_INCLUDED
