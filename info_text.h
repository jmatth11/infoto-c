#ifndef INFOTO_INFO_TEXT_H
#define INFOTO_INFO_TEXT_H

#include <stdlib.h>

typedef struct info_text {
  char **buffer;
  size_t size;
  char *separator;
} info_text;

/**
 * Initialize Info Text object.
 *
 * @param[in] info Info text object to initialize
 * @param[in] size The size to initialize to
 * @param[in] sep The separator for each value
 * @return 1 for success, 0 if malloc failed.
 */
int info_text_init(info_text *info, size_t size, char *sep);

/**
 * Free the internal strings and array.
 *
 * @param[in] info The info text object
 */
void info_text_free(info_text *info);

#endif