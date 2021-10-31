#include <stdio.h>

#include "config.h"
#include "exif.h"
#include "info_text.h"
#include "jpeg_handler.h"
#include "json_parsing.h"
#include "ttf_util.h"

// TODO
// - replace bool and int return values with error_report objects
// - clean up code.
// - create string structure to clean up string creation.
// - revisit ttf glyph writing to find the proper way to handle kerning and
//   white space.
//
// extra
// - explore pulling EXIF data out of PNG files.

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "please supply a jpeg file.\n");
    return 1;
  }
  config cfg;
  if (!config_from_json_file(argv[1], &cfg)) {
    fprintf(stderr, "generating config object from json file failed\n");
    return 1;
  }
  info_text info;
  infoto_info_text_init(&info, cfg.metadata.len, " | ");
  if (!infoto_read_exif_data(&cfg, &info)) {
    fprintf(stderr, "reading exif data failed.\n");
    return 1;
  }
  infoto_font_handler *font_handler;
  if (!infoto_font_handler_init(&font_handler)) {
    fprintf(stderr, "failed to initialize font library\n");
    return 1;
  }
  if (!infoto_font_handler_load_font(font_handler, cfg.font.ttf_file,
                                     cfg.font.point)) {
    fprintf(stderr, "failed to load ttf file.\n");
    return 1;
  }
  infoto_img_handler handler;
  infoto_jpeg_handler_init(&handler, font_handler);
  if (!handler.write_image(&handler, cfg.img, cfg.background, &info)) {
    fprintf(stderr, "failed adding text to image.\n");
  }
  infoto_info_text_free(&info);
  infoto_font_handler_free(&font_handler);
  infoto_jpeg_handler_free(&handler);
  infoto_free_config(&cfg);
  return 0;
}
