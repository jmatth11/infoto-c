#include "exif.h"
#include "config.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libexif/exif-content.h>
#include <libexif/exif-data.h>
#include <libexif/exif-tag.h>

#define FORMATTED_STRING_LEN (CONFIG_INFO_FIX_LEN * 2)

bool read_exif_data(config *cfg, info_text *output) {
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
  for (int i = 0; i < cfg->metadata.len; ++i) {
    // get metadata info object
    metadata_info mi;
    get_metadata_array(&cfg->metadata, i, &mi);
    // grab the tag name
    ExifTag tag = exif_tag_from_name(mi.name);
    if (tag == 0) {
      printf("tag not found. searching for: %s\n", mi.name);
      continue;
    }
    // grab the entry
    // this object is owned by the EXIF data object, do not unref
    ExifEntry *entry = exif_data_get_entry(exif, tag);
    if (entry == NULL) {
      printf("failed to get %s entry\n", mi.name);
      return false;
    }
    char *value = (char *)malloc(sizeof(char) * entry->size);
    // get the human readable value out
    exif_entry_get_value(entry, value, entry->size);
    const int buffer_size = (FORMATTED_STRING_LEN + entry->size);
    // + 1 for null terminator
    char *buffer = (char *)malloc((sizeof(char) * buffer_size) + 1);
    // get formated string
    const int buffer_N =
        sprintf(buffer, "%s%s%s", mi.prefix, value, mi.postfix);
    // free value since no longer needed
    free(value);
    // uppercase everything
    for (int j = 0; j < buffer_N; ++j) {
      buffer[j] = toupper(buffer[j]);
    }
    // ensure null terminated
    buffer[buffer_N] = '\0';
    output->buffer[i] = buffer;
  }
  return true;
}
