#include "config.h"
#include "error_codes.h"
#include "info_text.h"

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
