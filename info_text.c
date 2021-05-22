#include "info_text.h"

/**
 * Initialize Info Text object.
 *
 * @param[in] info Info text object to initialize
 * @param[in] size The size to initialize to
 * @param[in] sep The separator for each value
 * @return 1 for success, 0 if malloc failed.
 */
int info_text_init(info_text *info, size_t size, char *sep) {
  char **tmp = (char **)malloc(sizeof(char *) * size);
  if (tmp == NULL) {
    return 0;
  }
  info->buffer = tmp;
  info->separator = sep;
  info->size = size;
  return 1;
}

/**
 * Get the formatted string for the info text object.
 *
 * @param[in] info The info text object
 * @return The formatted string
 */
char *info_text_to_string(info_text *info) {
  char *buffer = NULL;
  return buffer;
}

/**
 * Free the internal strings and array.
 *
 * @param[in] info The info text object
 */
void info_text_free(info_text *info) {
  for (int i = 0; i < info->size; ++i) {
    free(info->buffer[i]);
  }
  free(info->buffer);
  info->buffer = NULL;
}
