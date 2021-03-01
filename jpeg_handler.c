#include "jpeg_handler.h"

#include <stdio.h>
#include <jpeglib.h>
#include <setjmp.h>

struct jpeg_err {
    struct jpeg_error_mgr pub;
    
    jmp_buf jmp_to_err_handler;
};

static void handle_read_error(j_common_ptr cinfo) {
    struct jpeg_err* err = (struct jpeg_err*)cinfo->err;
        
    (*cinfo->err->output_message)(cinfo);

    longjmp(err->jmp_to_err_handler, 1);
}

bool read_img(const char *file_name) {
    struct jpeg_decompress_struct cinfo;

    struct jpeg_err msg_err;

    FILE *jpeg_file;
    JSAMPARRAY buffer;
    int row_stride;

    if ((jpeg_file = fopen(file_name, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", file_name);
        return false;
    }
    
    cinfo.err = jpeg_std_error(&msg_err.pub);
    msg_err.pub.error_exit = handle_read_error;

    if (setjmp(msg_err.jmp_to_err_handler)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(jpeg_file);
        return false;
    }

    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, jpeg_file);
    
    jpeg_read_header(&cinfo, true);

    jpeg_start_decompress(&cinfo);

    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

    while(cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
    }

    jpeg_finish_decompress(&cinfo);

    jpeg_destroy_decompress(&cinfo);

    fclose(jpeg_file);

    return true;
}
