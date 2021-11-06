#include "config.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

void infoto_init_config(config *cfg) { init_metadata_array(&cfg->metadata, 1); }

void infoto_free_config(config *cfg) {
  free(cfg->img);
  free(cfg->font.ttf_file);
  free_metadata_array(&cfg->metadata);
}

void infoto_print_config(const config *cfg) {
  if (cfg == NULL) {
    printf("config structure was NULL for print\n");
    return;
  }
  printf("config structure: {\n");
  printf("img: \"%s\"\n", cfg->img);
  printf("font: {\n");
  if (cfg->font.ttf_file == NULL) {
    printf("config font ttf_file was null\n");
    return;
  }
  printf("\tttf_file: %s\n", cfg->font.ttf_file);
  printf("\tcolor: %d\n", cfg->font.color);
  printf("\tpoint: %d\n", cfg->font.point);
  printf("}\n");
  printf("background: {\n");
  printf("\tcolor: %d\n", cfg->background.color);
  printf("\tpixels: %d\n", cfg->background.pixels);
  printf("}\n");
  printf("metadata: [\n");
  for (int i = 0; i < cfg->metadata.len; ++i) {
    metadata_info info;
    get_metadata_array(&cfg->metadata, i, &info);
    printf("\t{\n");
    printf("\t\tname: %s\n", info.name);
    printf("\t\tpostfix: %s\n", info.postfix);
    printf("\t\tprefix: %s\n", info.prefix);
    printf("\t{\n");
  }
  printf("]\n}\n");
}
