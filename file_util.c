#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include "file_util.h"
#include "str_utils.h"

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
        char *filename = NULL;
        size_t file_len = strlen(in_file->d_name);
        size_t length = dir_len + file_len;
        infoto_inc_string_size(&filename, length);
        strncpy(filename, path, dir_len);
        strncpy(&filename[dir_len], in_file->d_name, file_len);
        insert_string_array(file_names, filename);
    }

    closedir(fd);

    return 0;
}
