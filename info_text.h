#ifndef INFOTO_INFO_TEXT_H
#define INFOTO_INFO_TEXT_H

#include <stdlib.h>

#include "error_codes.h"

typedef struct {
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
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
infoto_error_enum infoto_info_text_init(info_text *info, size_t size,
                                        char *sep);

/**
 * Get the formatted string for the info text object.
 *
 * @param[in] info The info text object
 * @return The formatted string
 */
char *infoto_info_text_to_string(const info_text *info);

/**
 * Free the internal strings and array.
 *
 * @param[in,out] info The info text object
 */
void infoto_info_text_free(info_text *info);

#endif
