#ifndef INFOTO_STR_UTILS
#define INFOTO_STR_UTILS

#include <stddef.h>

/**
 * Get filename extension starting pointer.
 *
 * @param[in] filename The filename to get extension for.
 * @return Pointer to filename extension.
 */
const char *get_filename_ext(const char *filename);

/**
 * Increase the size of the buffer string to the given size.
 *
 * @param[out] str The string to increase
 * @param[in] size The size to increase to
 * @return The number of bytes written (including null terminator, -1 if failed
 */
int inc_string_size(char **str, size_t size);

#endif
