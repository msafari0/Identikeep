#include "Files.h"

#include <string.h> 

const char *get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

std::vector<std::string> getPathList(std::string initialPath) {
    std::vector<std::string> dList;
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (initialPath.c_str())) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
        std::string ext(get_filename_ext(ent->d_name));
        if (ext == "dll" || ext == "so")
            dList.push_back(initialPath+"/"+std::string(ent->d_name));   /* NOT PORTABLE */
      }
      closedir (dir);
    } else {
      /* could not open directory */
      perror ("");
    }
    return dList;
}

