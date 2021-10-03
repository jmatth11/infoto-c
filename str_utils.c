#include "str_utils.h"

#include <stdlib.h>
#include <string.h>

/**
 * Get filename extension starting pointer.
 *
 * @param[in] filename The filename to get extension for.
 * @return Pointer to filename extension.
 */
const char *get_filename_ext(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if (!dot || dot == filename)
    return "";
  return dot + 1;
}

/**
 * Increase the size of the buffer string to the given size.
 *
 * @param[out] str The string to increase
 * @param[in] size The size to increase to
 * @return The number of bytes written (including null terminator, -1 if failed
 */
int inc_string_size(char **str, size_t size) {
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
