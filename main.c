#include <stdio.h>

#include "config.h"
#include "exif.h"
#include "jpeg_handler.h"
#include "json_parsing.h"

// TODO
// - pull json lib to parse config file.
// - iterate over TAG names to pull info out.? maybe done, look back over
// - create border around original image.
// - add text with info to newly bordered image.
//
// extra
// - explore pulling EXIF data out of PNG files.

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    printf("please supply a jpeg file.\n");
    return 1;
  }
  struct config cfg;
  if (!config_from_json_file(argv[1], &cfg)) {
    printf("generating config object from json file failed\n");
    return 1;
  }
  print_config(&cfg);
  char(*buffer)[50];
  buffer = malloc(cfg.metadata.len * sizeof(*buffer));
  if (!read_exif_data(&cfg, buffer)) {
    printf("reading exif data failed.\n");
    return 1;
  }
  for (int i = 0; i < cfg.metadata.len; ++i) {
    printf("value: %s\n", buffer[i]);
  }
  free_config(&cfg);
  free(buffer);
  return 0;
}
