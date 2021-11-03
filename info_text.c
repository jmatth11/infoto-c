#include "info_text.h"

#include <string.h>

/**
 * Initialize Info Text object.
 *
 * @param[in] info Info text object to initialize
 * @param[in] size The size to initialize to
 * @param[in] sep The separator for each value
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
infoto_error_enum infoto_info_text_init(info_text *info, size_t size,
                                        char *sep) {
  char **tmp = (char **)malloc(sizeof(char *) * size);
  if (tmp == NULL) {
    return INFOTO_ERR_MALLOC;
  }
  info->buffer = tmp;
  info->separator = sep;
  info->size = size;
  return INFOTO_SUCCESS;
}

/**
 * Get the formatted string for the info text object.
 *
 * @param[in] info The info text object
 * @return The formatted string
 */
char *infoto_info_text_to_string(const info_text *info) {
  char *buffer = NULL;
  size_t sep_len = strlen(info->separator);
  size_t length = 0;
  for (int i = 0; i < info->size; ++i) {
    length += strlen(info->buffer[i]);
    length += sep_len;
  }
  // take off ending separator length, then add null character
  length = length - sep_len + 1;
  buffer = (char *)malloc(sizeof(char) * length);
  size_t str_pos = 0;
  for (int i = 0; i < info->size; ++i) {
    char *value = info->buffer[i];
    size_t value_len = strlen(value);
    memcpy(&buffer[str_pos], value, value_len);
    str_pos += value_len;
    char *sep = info->separator;
    size_t sep_len = strlen(sep);
    memcpy(&buffer[str_pos], sep, sep_len);
    str_pos += sep_len;
  }
  buffer[length - 1] = '\0';
  return buffer;
}

/**
 * Free the internal strings and array.
 *
 * @param[in] info The info text object
 */
void infoto_info_text_free(info_text *info) {
  for (int i = 0; i < info->size; ++i) {
    free(info->buffer[i]);
  }
  free(info->buffer);
  info->buffer = NULL;
}
