#ifndef INFOTO_EXIF_H
#define INFOTO_EXIF_H

#include "config.h"
#include "deps/array_template/array_template.h"
#include "error_codes.h"
#include "info_text.h"
#include <libexif/exif-tag.h>

/**
 * Simple EXIF data structure to hold EXIF tag info.
 */
typedef struct infoto_exif_data {
  const char *name;
  ExifTag value;
} infoto_exif_data;

generate_array_template(infoto_exif_data, infoto_exif_data);

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

/**
 * Read all EXIF tag names the given image has.
 *
 * @param[in] image_name The image to read EXIF tags from.
 * @param[in,out] data_arr The EXIF data array to populate.
 * @returns INFOTO_SUCCESS if successful, otherwise an error code.
 */
infoto_error_enum infoto_read_all_exif_tags(const char *image_name,
                                            infoto_exif_data_array *data_arr);

#endif
