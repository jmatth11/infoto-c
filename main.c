#include <stdio.h>

#include "config.h"
#include "exif.h"
#include "info_text.h"
#include "jpeg_handler.h"
#include "json_parsing.h"

// TODO
// - create border around original image.
// - add text with info to newly bordered image.
// - replace bool and int return values with error_report objects
//
// extra
// - explore pulling EXIF data out of PNG files.

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    printf("please supply a jpeg file.\n");
    return 1;
  }
  config cfg;
  if (!config_from_json_file(argv[1], &cfg)) {
    printf("generating config object from json file failed\n");
    return 1;
  }
  print_config(&cfg);
  info_text info;
  info_text_init(&info, cfg.metadata.len, " | ");
  if (!read_exif_data(&cfg, &info)) {
    printf("reading exif data failed.\n");
    return 1;
  }
  for (int i = 0; i < info.size; ++i) {
    printf("value: %s\n", info.buffer[i]);
  }
  if (!add_text_to_img(&cfg, &info)) {
    printf("failed adding text to image.\n");
  }
  free_config(&cfg);
  info_text_free(&info);
  return 0;
}
