#include <stdio.h>

#include "exif.h"
#include "jpeg_handler.h"

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        printf("please supply a jpeg file.\n");
        return 1;
    }
    read_exif_data(argv[1]);
    return 0;
}
