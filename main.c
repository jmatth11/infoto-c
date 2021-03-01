#include <stdio.h>

#include "exif.h"
#include "jpeg_handler.h"

// TODO
// - pull json lib to parse config file.
// - iterate over TAG names to pull info out.
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
    read_exif_data(argv[1]);
    return 0;
}
