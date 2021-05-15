#include "exif.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libexif/exif-content.h>
#include <libexif/exif-data.h>
#include <libexif/exif-tag.h>

#define ENTRY_VALUE_LEN 30
#define FORMATTED_STRING_LEN 50

bool read_exif_data(struct config *cfg, char (*output)[50]) {
  const char *file_name = cfg->img;
  // check the access of the file
  if (access(file_name, F_OK) != 0) {
    printf("file is not accessible: %s\n", file_name);
    return false;
  }
  // read out EXIF data
  ExifData *exif = exif_data_new_from_file(file_name);
  if (exif == NULL) {
    printf("could not read exif data for file: %s\n", file_name);
    return false;
  }
  // initialize buffers
  char buffer[FORMATTED_STRING_LEN];
  char value[ENTRY_VALUE_LEN];
  for (int i = 0; i < cfg->metadata.len; ++i) {
    // clear our buffers
    memset(buffer, 0, sizeof(buffer));
    memset(value, 0, sizeof(value));
    const struct metadata_info *mi = get_metadata_list(&cfg->metadata, i);
    // grab the tag name
    ExifTag tag = exif_tag_from_name(mi->name);
    if (tag == 0) {
      printf("tag not found. searching for: %s\n", mi->name);
      continue;
    }
    // grab the entry
    ExifEntry *entry = exif_data_get_entry(exif, tag);
    if (entry == NULL) {
      printf("failed to get %s entry\n", mi->name);
      return false;
    }
    // get the human readable value out
    exif_entry_get_value(entry, value, ENTRY_VALUE_LEN);
    // get formated string
    sprintf(buffer, "%s%s%s", mi->prefix, value, mi->postfix);
    // copy to output
    strncpy(output[i], buffer, FORMATTED_STRING_LEN);
    // decrement ref
    exif_entry_unref(entry);
  }
  return true;
}
