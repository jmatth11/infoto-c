#include <stdio.h>

#include "config.h"
#include "exif.h"
#include "file_util.h"
#include "info_text.h"
#include "jpeg_handler.h"
#include "json_parsing.h"
#include "process.h"
#include "ttf_util.h"

#ifndef INFOTO_VERSION
    #define INFOTO_VERSION "no_version"
#endif

int main(int argc, const char *argv[]) {
  printf("version: %s\n", INFOTO_VERSION);
  if (argc < 2) {
    fprintf(stderr, "please supply a jpeg file.\n");
    return 1;
  }
  info_text info;
  // read in config values
  config cfg;
  if (config_from_json_file(argv[1], &cfg) != INFOTO_SUCCESS) {
    fprintf(stderr, "generating config object from json file failed\n");
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
  // handle for directory
  if (is_dir(cfg.target)) {
    string_array filenames;
    init_string_array(&filenames, 1);
    if (grab_files_from_dir(cfg.target, &filenames) != 0) {
      fprintf(stderr, "reading images from directory failed.\n");
      return 1;
    }
    string_array out_names;
    init_string_array(&out_names, 1);
    if (
        infoto_process_bulk(
                &handler, cfg.background,
                cfg.font, &cfg.metadata, &filenames, &out_names) != INFOTO_SUCCESS) {
      fprintf(stderr, "processing bulk images failed.\n");
      return 1;
    }
    for (int i = 0; i < out_names.len; ++i) {
      fprintf(stdout, "Created file: %s\n", out_names.string_data[i]);
    }
    for (int i = 0; i < filenames.len; ++i) {
        free(filenames.string_data[i]);
        if (i < out_names.len) {
          free(out_names.string_data[i]);
        }
    }
    free_string_array(&filenames);
    free_string_array(&out_names);
  } else {
    // handle for single file.
    // generate info text for img
    infoto_info_text_init(&info, cfg.metadata.len, " | ");
    if (infoto_read_exif_data(cfg.target, &cfg.metadata, &info) != INFOTO_SUCCESS) {
      fprintf(stderr, "reading exif data failed.\n");
      return 1;
    }
    char *edited_img;
    if (handler.write_image(&handler, cfg.target, cfg.background, cfg.font, &info,
                            &edited_img) != INFOTO_SUCCESS) {
      fprintf(stderr, "failed adding text to image.\n");
      return 1;
    }
    printf("created edited image: %s\n", edited_img);
    free(edited_img);
  }
  // clean up
  infoto_info_text_free(&info);
  infoto_font_handler_free(&font_handler);
  infoto_jpeg_handler_free(&handler);
  infoto_free_config(&cfg);
  return 0;
}
