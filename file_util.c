#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include "file_util.h"
#include "str_utils.h"

#define PATH_SEPARATOR "/"
#define PATH_SEPARATOR_LEN 1


char* join_paths(const char *dir, size_t dir_len, const char *base, size_t base_len);

int is_dir(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0)
        return 0;
    return S_ISDIR(path_stat.st_mode);
}

int grab_files_from_dir(const char *path, string_array *file_names) {
    DIR *fd;
    struct dirent *in_file;
    size_t dir_len = strlen(path);

    if (NULL == (fd = opendir(path)))
        return 1;

    while((in_file = readdir(fd))) {
        if (!strcmp(in_file->d_name, ".")) continue;
        if (!strcmp(in_file->d_name, "..")) continue;
        size_t file_len = strlen(in_file->d_name);
        char * filename = join_paths(path, dir_len, in_file->d_name, file_len);
        insert_string_array(file_names, filename);
    }

    closedir(fd);

    return 0;
}

char* join_paths(const char *dir, size_t dir_len, const char *base, size_t base_len) {
   char *out = NULL;
   size_t length = dir_len + base_len + PATH_SEPARATOR_LEN;
   size_t path_len = dir_len + PATH_SEPARATOR_LEN;
   infoto_inc_string_size(&out, length);
   strncpy(out, dir, dir_len);
   if (strcmp(&dir[dir_len - PATH_SEPARATOR_LEN], PATH_SEPARATOR) != 0) {
     memcpy(&out[dir_len], PATH_SEPARATOR, PATH_SEPARATOR_LEN);
     memcpy(&out[path_len], base, base_len);
   } else {
     memcpy(&out[dir_len], base, base_len);
   }
   return out;
}
