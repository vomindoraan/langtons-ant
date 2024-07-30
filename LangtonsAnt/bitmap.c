// Adapted from: https://stackoverflow.com/a/47785639
#include "io.h"

#include <stdio.h>

/* 24-bit BGR (aka. little endian RGB) */
const pixel_t color_map[COLOR_COUNT] = {
	[COLOR_BLACK]   = { 0x00, 0x00, 0x00 },
	[COLOR_SILVER]  = { 0xAA, 0xAA, 0xAA },

	[COLOR_GRAY]    = { 0x55, 0x55, 0x55 },
	[COLOR_WHITE]   = { 0xFF, 0xFF, 0xFF },

	[COLOR_NAVY]    = { 0xAA, 0x00, 0x00 },
	[COLOR_GREEN]   = { 0x00, 0xAA, 0x00 },
	[COLOR_MAROON]  = { 0x00, 0x00, 0xAA },

	[COLOR_BLUE]    = { 0xFF, 0x00, 0x00 },
	[COLOR_LIME]    = { 0x00, 0xFF, 0x00 },
	[COLOR_RED]     = { 0x00, 0x00, 0xFF },

	[COLOR_TEAL]    = { 0xAA, 0xAA, 0x00 },
	[COLOR_PURPLE]  = { 0xAA, 0x00, 0xAA },
	[COLOR_OLIVE]   = { 0x00, 0xAA, 0xAA },

	[COLOR_AQUA]    = { 0xFF, 0xFF, 0x00 },
	[COLOR_FUCHSIA] = { 0xFF, 0x00, 0xFF },
	[COLOR_YELLOW]  = { 0x00, 0xFF, 0xFF },
};

static byte *init_file_header(int height, int stride)
{
	int file_size = FILE_HEADER_SIZE + INFO_HEADER_SIZE + (stride * height);
	static byte file_header[] = {
		0, 0,       // signature
		0, 0, 0, 0, // image file size in bytes
		0, 0, 0, 0, // reserved
		0, 0, 0, 0, // start of pixel array
	};

	file_header[0]  = (byte)('B');
	file_header[1]  = (byte)('M');
	file_header[2]  = (byte)(file_size);
	file_header[3]  = (byte)(file_size >> 8);
	file_header[4]  = (byte)(file_size >> 16);
	file_header[5]  = (byte)(file_size >> 24);
	file_header[10] = (byte)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);

	return file_header;
}

static byte *init_info_header(int height, int width)
{
	static byte info_header[] = {
		0, 0, 0, 0, // header size
		0, 0, 0, 0, // image width
		0, 0, 0, 0, // image height
		0, 0,       // number of color planes
		0, 0,       // bits per pixel
		0, 0, 0, 0, // compression
		0, 0, 0, 0, // image size
		0, 0, 0, 0, // horizontal resolution
		0, 0, 0, 0, // vertical resolution
		0, 0, 0, 0, // colors in color table
		0, 0, 0, 0, // important color count
	};

	info_header[0]  = (byte)(INFO_HEADER_SIZE);
	info_header[4]  = (byte)(width);
	info_header[5]  = (byte)(width >> 8);
	info_header[6]  = (byte)(width >> 16);
	info_header[7]  = (byte)(width >> 24);
	info_header[8]  = (byte)(height);
	info_header[9]  = (byte)(height >> 8);
	info_header[10] = (byte)(height >> 16);
	info_header[11] = (byte)(height >> 24);
	info_header[12] = (byte)(1);
	info_header[14] = (byte)(BYTES_PER_PIXEL * 8);

	return info_header;
}

int create_bitmap_file(const char *filename, pixel_t *image, size_t height, size_t width)
{
	FILE *output;
	size_t width_in_bytes = width * BYTES_PER_PIXEL;
	byte padding[3] = { 0, 0, 0 };
	size_t padding_size = (4 - width_in_bytes % 4) % 4;
	size_t stride = width_in_bytes + padding_size;
	size_t total_size = FILE_HEADER_SIZE + INFO_HEADER_SIZE
	                  + height * (width_in_bytes + padding_size);
	size_t i, e;

	if (!(output = fopen(filename, "wb"))) {
		return EOF;
	}

	byte *file_header = init_file_header(height, stride);
	e = fwrite(file_header, 1, FILE_HEADER_SIZE, output);

	byte *info_header = init_info_header(height, width);
	e += fwrite(info_header, 1, INFO_HEADER_SIZE, output);

	for (i = 0; i < height; i++) {
		e += fwrite(image+i*width, BYTES_PER_PIXEL, width, output);
		e += fwrite(padding, 1, padding_size, output);
	}

	if (fclose(output) == EOF || e < total_size) {
		return EOF;
	}
	return e;
}
