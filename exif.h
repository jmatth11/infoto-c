#ifndef INFOTO_EXIF_H
#define INFOTO_EXIF_H

#include "config.h"
#include "error_codes.h"
#include "info_text.h"

typedef struct infoto_exif_data {
  char *name;
  char *value;
} infoto_exif_data;

/**
 * Read EXIF data from JPEG file.
 *
 * @param[in] image_name The image to read EXIF data from.
 * @param[in] metadata The array of metadata info.
 * @param[out] output An info text buffer object
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
infoto_error_enum infoto_read_exif_data(const char *image_name,
                                        const metadata_array *metadata,
                                        info_text *output);

// TODO make out param of infoto_exif_data
infoto_error_enum infoto_read_all_exif_data(const char *image_name);

#endif
