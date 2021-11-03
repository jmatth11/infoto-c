#include "str_utils.h"

#include <stdlib.h>
#include <string.h>

#define EDITED_FILE_NAME "-edited"
#define EDITED_FILE_NAME_LEN strlen(EDITED_FILE_NAME)

/**
 * Get filename extension starting pointer.
 *
 * @param[in] filename The filename to get extension for.
 * @return Pointer to filename extension.
 */
const char *infoto_get_filename_ext(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return "";
  return dot;
}

/**
 * Increase the size of the buffer string to the given size.
 * This function adds +1 to size to handle adding null terminator.
 *
 * @param[out] str The string to increase
 * @param[in] size The size to increase to
 * @return The number of bytes written (including null terminator, -1 if failed
 */
int infoto_inc_string_size(char **str, size_t size) {
  const size_t char_size = sizeof(char);
  // extra + char_size for null terminator
  const size_t N = (char_size * size) + char_size;
  char *tmp = NULL;
  if (*str == NULL) {
    tmp = (char *)malloc(N);
  } else {
    tmp = (char *)realloc(*str, N);
  }
  if (tmp == NULL)
    return -1;
  tmp[N - 1] = '\0';
  *str = tmp;
  return N;
}

/**
 * Get a unique edit file name for the given filename.
 *
 * @param[in] filename The filename to derive new filename from.
 * @returns New filename to identify the edit file, NULL if failed.
 */
char *infoto_get_edit_file_name(const char *filename) {
  const char *start_of_extension = infoto_get_filename_ext(filename);
  // calculate lengths of strings
  int extension_len = strlen(start_of_extension);
  int img_file_name_len = strlen(filename);
  int file_name_no_ext_len = (img_file_name_len - extension_len);
  int edited_file_name_len = img_file_name_len + EDITED_FILE_NAME_LEN;
  char *edited_file_name = NULL;
  if (infoto_inc_string_size(&edited_file_name, edited_file_name_len) == -1) {
    return NULL;
  }
  strncpy(edited_file_name, filename, file_name_no_ext_len);
  memcpy(&edited_file_name[file_name_no_ext_len], EDITED_FILE_NAME,
         EDITED_FILE_NAME_LEN);
  memcpy(&edited_file_name[file_name_no_ext_len + EDITED_FILE_NAME_LEN],
         start_of_extension, extension_len);
  return edited_file_name;
}
