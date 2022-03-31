#include "process.h"
#include "exif.h"

/**
 * Process a bulk of images with the given background and font info.
 *
 * @param[in] handler The image handler.
 * @param[in] background The background info.
 * @param[in] font The font info.
 * @param[in] metadata The array of metadata info.
 * @param[in] imgs The array of image filenames.
 * @param[out] edited_imgs The array of edited image filenames.
 * @returns INFOTO_SUCCESS if successful, otherwise infoto_error_enum error.
 */
infoto_error_enum infoto_process_bulk(struct infoto_img_handler *handler,
                                      const background_info background,
                                      const font_info font,
                                      const metadata_array *metadata,
                                      const string_array *imgs,
                                      string_array *edited_imgs) {
  infoto_error_enum result = INFOTO_SUCCESS;
  for (int i = 0; i < imgs->len; ++i) {
    const char *image_name = imgs->string_data[i];
    // TODO evaluate glyph size compared to background border size
    // possibly return early with warning
    info_text info;
    infoto_info_text_init(&info, metadata->len, " | ");
    result = infoto_read_exif_data(image_name, metadata, &info);
    if (result != INFOTO_SUCCESS) {
      break;
    }
    char *out;
    result = handler->write_image(handler, image_name, background, font, &info,
                                  &out);
    if (result != INFOTO_SUCCESS) {
      break;
    }
    insert_string_array(edited_imgs, out);
    infoto_info_text_free(&info);
  }
  return result;
}
