#include "str_utils.h"

#ifndef INFOTO_FILE_UTIL_H
#define INFOTO_FILE_UTIL_H

/**
 * Check to see if path is a directory.
 * @param path The give path.
 * @returns zero if the path is a directory.
 */
int is_dir(const char *);

/**
 * Grab all files from a directory and populate them in the given string_array.
 * @param path The given directory.
 * @param file_names The structure to hold the filenames.
 * @returns 1 if failed, 0 if successful.
 */
int grab_files_from_dir(const char *, string_array *);

#endif
