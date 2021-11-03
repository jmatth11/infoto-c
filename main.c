#include <stdio.h>

#include "config.h"
#include "exif.h"
#include "info_text.h"
#include "jpeg_handler.h"
#include "json_parsing.h"
#include "ttf_util.h"

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "please supply a jpeg file.\n");
    return 1;
  }
  // read in config values
  config cfg;
  if (config_from_json_file(argv[1], &cfg) != INFOTO_SUCCESS) {
    fprintf(stderr, "generating config object from json file failed\n");
    return 1;
  }
  // generate info text for img
  info_text info;
  infoto_info_text_init(&info, cfg.metadata.len, " | ");
  if (infoto_read_exif_data(&cfg, &info) != INFOTO_SUCCESS) {
    fprintf(stderr, "reading exif data failed.\n");
    return 1;
  }
  // initialize and load our font
  infoto_font_handler *font_handler;
  if (infoto_font_handler_init(&font_handler) != INFOTO_SUCCESS) {
    fprintf(stderr, "failed to initialize font library\n");
    return 1;
  }
  if (infoto_font_handler_load_font(font_handler, cfg.font.ttf_file,
                                    cfg.font.point) != INFOTO_SUCCESS) {
    fprintf(stderr, "failed to load ttf file.\n");
    return 1;
  }
  // initialize and write out the edited jpeg file
  infoto_img_handler handler;
  infoto_jpeg_handler_init(&handler, font_handler);
  int result = 0;
  if (handler.write_image(&handler, cfg.img, cfg.background, &info) !=
      INFOTO_SUCCESS) {
    fprintf(stderr, "failed adding text to image.\n");
    result = 1;
  }
  // clean up
  infoto_info_text_free(&info);
  infoto_font_handler_free(&font_handler);
  infoto_jpeg_handler_free(&handler);
  infoto_free_config(&cfg);
  return result;
}
