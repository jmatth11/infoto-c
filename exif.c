#include "exif.h"

#include <stdio.h>
#include <unistd.h>

#include <libexif/exif-data.h>
#include <libexif/exif-content.h>
#include <libexif/exif-tag.h>


bool read_exif_data(const char *file_name) {
    if (access(file_name, F_OK) != 0) {
        printf("file is not accessible: %s\n", file_name);
        return false;
    }
    ExifData* exif = exif_data_new_from_file(file_name);
    if (exif == NULL) {
        printf("could not read exif data for file: %s\n", file_name);
        return false;
    }
    // TODO utilize exif_tag_from_name function
    ExifEntry* entry = exif_data_get_entry(exif, EXIF_TAG_MODEL);

    if (entry == NULL) {
        printf("failed to get FNUMBER entry\n");
        return false;
    }
    
    printf("entry value for %s = %s\n", exif_tag_get_name(entry->tag), entry->data);

    return true;
}
