// Utility to load an indexed-color PNG file and emit XOBJ_FRM_DAT and
// XOBJ_REF_DAT files.
//
// Some notes on XSP:
//
// XSP is initialized with data passed in by the user. This data contains:
// * Sprite PCG texture data (XSP, SP)
// * Metasprite definition data (FRM)
// * List of metasprite definitions (REF)
//
// XSP can either draw a single hardware sprite (SP), or a complex metasprite
// composed of multiple hardware sprites (in XSP parliance, an XOBJ).
//
// To draw a hardware sprite (SP), only texture data is required.
//
// For XOBJ drawing, XSP allows the user to specify what is to be drawn by a
// single pattern number. FRM definitions exist to provide instructions on how
// to compose an XOBJ pattern from multiple hardware sprites. The REF data
// indexes within FRM data to note where definitions start and end for a frame.
//
// Looking at this, you may wonder "Why not have one small set of XSP, FRM, and
// REF files for a single frame, and load them one at a time?" More than
// anything else, the reason is that PCG data may be reused between different
// metasprites. It would be a waste of memory and load time to duplicate tile
// data between multiple frames that barely have any changes in them.
//
// So, in general, drawing or generating an indexed .png of a sprite sheet is
// recommended, as corresponding XSP, FRM, and REF data can be efficiently
// emitted from it.
//
// For this program, rather than require a metadata file that specifies sprite
// size and clipping regions for each one, I've opted for a simple design that
// operates on a fixed sprite size for the whole sheet. The program will omit
// unused space, so feel free to edit enormous sprites that don't use most of
// their frame.
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "lodepng.h"

#define PCG_TILE_PX 16

typedef enum ConvOrigin
{
	CONV_ORIGIN_LEFT_TOP,
	CONV_ORIGIN_CENTER_TOP,
	CONV_ORIGIN_RIGHT_TOP,
	CONV_ORIGIN_LEFT_CENTER,
	CONV_ORIGIN_CENTER_CENTER,
	CONV_ORIGIN_RIGHT_CENTER,
	CONV_ORIGIN_LEFT_BOTTOM,
	CONV_ORIGIN_CENTER_BOTTOM,
	CONV_ORIGIN_RIGHT_BOTTOM,
	CONV_ORIGIN_DEFAULT = CONV_ORIGIN_CENTER_CENTER
} ConvOrigin;

typedef enum ConvMode
{
	CONV_MODE_AUTO,
	CONV_MODE_XOBJ,
	CONV_MODE_SP,
	CONV_MODE_DEFAULT = CONV_MODE_AUTO
} ConvMode;

//
// Output files
//

// PCG output files and relevant indexing variables.
static FILE *sf_pcg_out = NULL;  // XSP or SP PCG sprite data.
static FILE *sf_frm_out = NULL;  // XSP_FRM_DAT data.
static FILE *sf_ref_out = NULL;  // XPS_REF_DAT data.

static uint8_t *s_pcg_dat;  // Allocated to the max sprite count.
static int s_pcg_count = 0;
static uint32_t s_frm_offs = 0;

static bool init_file_handles(ConvMode mode, const char *outname)
{
	s_pcg_count = 0;
	s_frm_offs = 0;

	char fname_buffer[256];

	snprintf(fname_buffer, sizeof(fname_buffer), (mode == CONV_MODE_XOBJ) ? "%s.xsp" : "%s.sp", outname);
	sf_pcg_out = fopen(fname_buffer, "wb");
	if (!sf_pcg_out)
	{
		printf("Couldn't open %s for writing.\n", fname_buffer);
		return false;
	}

	if (mode == CONV_MODE_XOBJ)
	{
		snprintf(fname_buffer, sizeof(fname_buffer), "%s.frm", outname);
		sf_frm_out = fopen(fname_buffer, "wb");
		if (!sf_frm_out)
		{
			printf("Couldn't open %s for writing.\n", fname_buffer);
			fclose(sf_pcg_out);
			return false;
		}

		snprintf(fname_buffer, sizeof(fname_buffer), "%s.ref", outname);
		sf_ref_out = fopen(fname_buffer, "wb");
		if (!sf_frm_out)
		{
			printf("Couldn't open %s for writing.\n", fname_buffer);
			fclose(sf_pcg_out);
			fclose(sf_frm_out);
			return false;
		}
	}
	return true;
}

// Motorola 68000, and therefore XSP, uses big-endian data.
static void fwrite_uint16be(uint16_t val, FILE *f)
{
	uint8_t buf[2];
	buf[0] = (val >> 8) & 0xFF;
	buf[1] = val & 0xFF;
	fwrite(buf, 1, sizeof(buf), f);
	fflush(f);
}

static void fwrite_int16be(int16_t val, FILE *f)
{
	fputc((val >> 8) & 0xFF, f);
	fputc(val & 0xFF, f);
}

static void fwrite_uint32be(uint32_t val, FILE *f)
{
	fwrite_uint16be((val >> 16) & 0xFFFF, f);
	fwrite_uint16be(val & 0xFFFF, f);
}

//
// Configuration and init sanity
//

static ConvMode conv_mode_from_args(int argc, char **argv)
{
	if (argc < 6) return CONV_MODE_DEFAULT;
	const char arg = argv[5][0];
	switch (arg)
	{
		case 'a':
		case 'A':
			return CONV_MODE_AUTO;
		case 'x':
		case 'X':
			return CONV_MODE_XOBJ;
		case 's':
		case 'S':
			return CONV_MODE_SP;
		default:
			printf("Unrecognized mode \'%c\'; using CONV_MODE_AUTO.\n", arg);
			return CONV_MODE_AUTO;
	}
}

static ConvOrigin conv_origin_from_args(int argc, char **argv)
{
	ConvOrigin ret = CONV_ORIGIN_DEFAULT;
	if (argc < 7) return ret;
	const char *argstr = argv[6];
	if (strlen(argv[6]) < 2)
	{
		printf("Warning: Invalid origin '%s'; need two characters.\n", argstr);
		return ret;
	}

	switch (argstr[0])
	{
		case 'l':
		case 'L':
			ret = CONV_ORIGIN_LEFT_TOP;
			break;
		case 'c':
		case 'C':
			ret = CONV_ORIGIN_CENTER_TOP;
			break;
		case 'r':
		case 'R':
			ret = CONV_ORIGIN_RIGHT_TOP;
			break;
		default:
			printf("Warning: Unhandled X origin argument '%c'.\n", argstr[0]);
			return ret;
	}

	switch (argstr[1])
	{
		case 't':
		case 'T':
			switch (ret)
			{
				case CONV_ORIGIN_LEFT_TOP:
					ret = CONV_ORIGIN_LEFT_TOP;
					break;
				case CONV_ORIGIN_CENTER_TOP:
					ret = CONV_ORIGIN_CENTER_TOP;
					break;
				case CONV_ORIGIN_RIGHT_TOP:
					ret = CONV_ORIGIN_RIGHT_TOP;
					break;
				case CONV_ORIGIN_LEFT_CENTER:
					ret = CONV_ORIGIN_LEFT_TOP;
					break;
				case CONV_ORIGIN_CENTER_CENTER:
					ret = CONV_ORIGIN_CENTER_TOP;
					break;
				case CONV_ORIGIN_RIGHT_CENTER:
					ret = CONV_ORIGIN_RIGHT_TOP;
					break;
				case CONV_ORIGIN_LEFT_BOTTOM:
					ret = CONV_ORIGIN_LEFT_TOP;
					break;
				case CONV_ORIGIN_CENTER_BOTTOM:
					ret = CONV_ORIGIN_CENTER_TOP;
					break;
				case CONV_ORIGIN_RIGHT_BOTTOM:
					ret = CONV_ORIGIN_RIGHT_TOP;
					break;
			}
			break;
		case 'c':
		case 'C':
			switch (ret)
			{
				case CONV_ORIGIN_LEFT_TOP:
					ret = CONV_ORIGIN_LEFT_CENTER;
					break;
				case CONV_ORIGIN_CENTER_TOP:
					ret = CONV_ORIGIN_CENTER_CENTER;
					break;
				case CONV_ORIGIN_RIGHT_TOP:
					ret = CONV_ORIGIN_RIGHT_CENTER;
					break;
				case CONV_ORIGIN_LEFT_CENTER:
					ret = CONV_ORIGIN_LEFT_CENTER;
					break;
				case CONV_ORIGIN_CENTER_CENTER:
					ret = CONV_ORIGIN_CENTER_CENTER;
					break;
				case CONV_ORIGIN_RIGHT_CENTER:
					ret = CONV_ORIGIN_RIGHT_CENTER;
					break;
				case CONV_ORIGIN_LEFT_BOTTOM:
					ret = CONV_ORIGIN_LEFT_CENTER;
					break;
				case CONV_ORIGIN_CENTER_BOTTOM:
					ret = CONV_ORIGIN_CENTER_CENTER;
					break;
				case CONV_ORIGIN_RIGHT_BOTTOM:
					ret = CONV_ORIGIN_RIGHT_CENTER;
					break;
			}
			break;
		case 'b':
		case 'B':
			switch (ret)
			{
				case CONV_ORIGIN_LEFT_TOP:
					ret = CONV_ORIGIN_LEFT_BOTTOM;
					break;
				case CONV_ORIGIN_CENTER_TOP:
					ret = CONV_ORIGIN_CENTER_BOTTOM;
					break;
				case CONV_ORIGIN_RIGHT_TOP:
					ret = CONV_ORIGIN_RIGHT_BOTTOM;
					break;
				case CONV_ORIGIN_LEFT_CENTER:
					ret = CONV_ORIGIN_LEFT_BOTTOM;
					break;
				case CONV_ORIGIN_CENTER_CENTER:
					ret = CONV_ORIGIN_CENTER_BOTTOM;
					break;
				case CONV_ORIGIN_RIGHT_CENTER:
					ret = CONV_ORIGIN_RIGHT_BOTTOM;
					break;
				case CONV_ORIGIN_LEFT_BOTTOM:
					ret = CONV_ORIGIN_LEFT_BOTTOM;
					break;
				case CONV_ORIGIN_CENTER_BOTTOM:
					ret = CONV_ORIGIN_CENTER_BOTTOM;
					break;
				case CONV_ORIGIN_RIGHT_BOTTOM:
					ret = CONV_ORIGIN_RIGHT_BOTTOM;
					break;
			}
			break;
		default:
			printf("Warning: Unhandled Y origin argument '%c'.\n", argstr[1]);
			return ret;
	}

	return ret;
}

static void show_usage(const char *prog_name)
{
	printf("Usage: %s sprites.png w h outname <m=a> <o=cc>\n", prog_name);
	printf("      w: Width of sprite within spritesheet (decimal or hex)\n");
	printf("      h: Height of sprite within spritesheet (decimal or hex)\n");
	printf("outname: Base file path and name for output.\n");
	printf("      m: Mode (a = auto, x = xobj, s = sp)\n");
	printf("      o: Origin (XY, where both characters form an argument\n");
	printf("         t/l: top/left\n");
	printf("           c: center\n");
	printf("         b/r: bottom/right\n");
	printf("\n");
	printf("Example:\n");
	printf("    %s player.png 32 48 out/player x cb\n", prog_name);
	printf("\n");
	printf("The result is player.png being chopped into a series of 32x48\n");
	printf("XOBJ sprites with the origin at the center-bottom of the frame.\n");
}

static bool check_arg_sanity(int argc, char **argv)
{
	if (argc < 5)
	{
		show_usage(argv[0]);
		return false;
	}

	const int frame_w = strtoul(argv[2], NULL, 0);
	const int frame_h = strtoul(argv[3], NULL, 0);
	if (frame_w < PCG_TILE_PX || frame_h < PCG_TILE_PX)
	{
		printf("Frame sizes under 16 x 16 are not supported.\n");
		return -1;
	}
	return true;
}

// Free after usage. NULL on error.
static uint8_t *load_png_data(const char *fname,
                              unsigned int *png_w, unsigned int *png_h)
{
	uint8_t *ret;
	const unsigned int error = lodepng_decode_file(&ret, png_w, png_h,
	                                               fname, LCT_PALETTE, 8);
	if (error)
	{
		printf("LodePNG error %u: %s\n", error, lodepng_error_text(error));
		return NULL;
	}

	printf("Loaded \"%s\": %d x %d\n", fname, *png_w, *png_h);
	return ret;
}

//
// Sprite chopping
//

// Commits a metasprite to the REF_DAT file.
// sp_count: hardware sprites used in metasprite
// frm_offs: offset within FRM_DAT file for this metasprite
static void add_ref_dat(uint16_t sp_count, uint32_t frm_offs)
{
	fwrite_uint16be(sp_count, sf_ref_out);
	fwrite_uint32be(frm_offs, sf_ref_out);
	fwrite_uint16be(0x0000, sf_ref_out);  // Reserved / padding.
}

static void add_frm_dat(int16_t vx, int16_t vy, uint16_t pt, uint16_t rv)
{
	fwrite_int16be(vx, sf_frm_out);
	fwrite_int16be(vy, sf_frm_out);
	fwrite_uint16be(pt, sf_frm_out);
	fwrite_uint16be(rv, sf_frm_out);
//	printf("frm: %04d %04d %04d %04d \t$%04X%04X%04X%04X\n", vx, vy, pt, rv, vx, vy, pt, rv);
	s_frm_offs += 8;
}

// src points to a 128 byte chunk of PCG data
static void add_pcg_dat(const uint8_t *src)
{
	memcpy(&s_pcg_dat[s_pcg_count * 128], src, 128);
//	fwrite(src, 1, 128, sf_pcg_out);
	s_pcg_count++;
}

// src points to a 128 byte chunk of PCG data
// returns 0-65535 if PCG was found in PCG bank
// otherwise returns -1
// TODO: Consider storing hashes of the tiles alongside this and eliminating s_pcg_dat
static int find_pcg_dat(const uint8_t *src)
{
	for (int i = 0; i < s_pcg_count; i++)
	{
		const uint8_t *candidate = &s_pcg_dat[i * 128];
		if (memcmp(candidate, src, 128) == 0) return i;
	}
	return -1;
}

// Hunt top-down, then left-right, for the first viable sprite.
// Returns false if no image data was found.
static bool claim(const uint8_t *imgdat, int iw, int ih, int sx, int sy, int sw, int sh,
                  int *col, int *row)
{
	// Find the topmost row.
	*row = -1;
	for (int y = sy; y < sy + sh; y++)
	{
		for (int x = sx; x < sx + sw; x++)
		{
			if (imgdat[x + (y * iw)] == 0) continue;
			*row = y;
			break;
		}
		if (*row >= 0) break;
	}
	if (*row < 0) return false;  // Image is empty.

	// Scan rightwards to find the left edge of the sprite.
	*col = -1;
	for (int x = sx; x < sx + sw; x++)
	{
		const int ylim = (*row + PCG_TILE_PX < sh) ? (*row + PCG_TILE_PX) : sh;
		for (int y = *row; y < ylim; y++)
		{
			if (imgdat[x + (y * iw)] == 0) continue;
			*col = x;
			break;
		}
		if (*col >= 0) break;
	}
	if (*col < 0)
	{
		printf("Unexpectedly empty strip from row %d?\n", *row);
		return false;
	}

	return true;
}

// Takes the 8x8 tile from imgdat and places it in the appropriate 4bpp format
// into out. The data is erased from imgdat as it is taken.
// It is a given that imgdat is large enough for the indicated region.
// Data exceeding sw and sh is excluded.
static void clip_8x8_tile(uint8_t *imgdat, int iw, int sx, int sy, int limx, int limy, uint8_t *out)
{
	for (int y = 0; y < 8; y++)
	{
		uint8_t *line = &imgdat[sx + ((sy + y) * iw)];
		const int source_y = sy + y;
		for (int x = 0; x < 8; x += 2)
		{
			const int source_x = sx + x;
			uint8_t px[2] = {0, 0};
			if (source_y < limy)
			{
				if (source_x < limx)
				{
					px[0] = (line[x] & 0xF);
					line[x] = 0;
				}
				if (source_x + 1 < limx)
				{
					px[1] = (line[x + 1] & 0xF);
					line[x + 1] = 0;
				}
			}
			*out++ = (px[0] << 4) | px[1];
		}
	}
}

static void origin_for_sp(ConvOrigin origin, int sw, int sh, int *ox, int *oy)
{
	switch (origin)
	{
		case CONV_ORIGIN_LEFT_TOP:      *oy = 0;      *ox = 0;      break;
		case CONV_ORIGIN_CENTER_TOP:    *oy = 0;      *ox = sw / 2; break;
		case CONV_ORIGIN_RIGHT_TOP:     *oy = 0;      *ox = sw - 1; break;
		case CONV_ORIGIN_LEFT_CENTER:   *oy = sh / 2; *ox = 0;      break;
		case CONV_ORIGIN_CENTER_CENTER: *oy = sh / 2; *ox = sw / 2; break;
		case CONV_ORIGIN_RIGHT_CENTER:  *oy = sh / 2; *ox = sw - 1; break;
		case CONV_ORIGIN_LEFT_BOTTOM:   *oy = sh - 1; *ox = 0;      break;
		case CONV_ORIGIN_CENTER_BOTTOM: *oy = sh - 1; *ox = sw / 2; break;
		case CONV_ORIGIN_RIGHT_BOTTOM:  *oy = sh - 1; *ox = sw - 1; break;
	}
}

static void render_region(uint8_t *imgdat, int iw, int ih, int sx, int sy, int sw, int sh)
{
	for (int x = sx; x < sx + sw; x++) printf("--");
	printf("\n");
	for (int y = sy; y < sy + sh; y++)
	{
		for (int x = sx; x < sx + sw; x++)
		{
			static const char hex[0x10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
			const uint8_t px = imgdat[x + (y * iw)];
			printf("%c ", px ? hex[px] : ((x % 8) == 0 || (y % 8) == 0) ? '.' : ' ');
		}
		printf("|\n");
	}
	for (int x = sx; x < sx + sw; x++) printf("--");
	printf("\n");
}

// Takes sprite data from imgdat and generates XSP entry data for it.
// Adds to the PCG, FRM, and REF files as necessary.
static void chop_sprite(uint8_t *imgdat, int iw, int ih, ConvMode mode, ConvOrigin origin,
                        int sx, int sy, int sw, int sh)
{
	// Data that gets placed into the ref dat at the end.
	// frm_offs needs to point at the start of the XOBJ_FRM_DAT for this
	// sprite. s_frm_offs will be added for every hardware sprite chopped
	// out from the metasprite data.
	uint16_t sp_count = 0;  // SP count in REF dat
	const uint32_t frm_offs = s_frm_offs;

	int ox, oy;
	origin_for_sp(origin, sw, sh, &ox, &oy);

	// If the sprite area from imgdat isn't empty:
	// 1) Search existing PCG data, see if we have the image data already.
	//    Do check for X and Y mirrored versions as well.
	//    If we already have it,
	//      a) store position in PCG data / 128 to get pattern index
	//      b) record X/Y mirroring if used to place the sprite.
	//    If we don't have it,
	//      a) store s_pcg_count as the pattern index
	//      b) call add_pcg_dat for the sprite data.
	// 1.5) If in SP mode, skip to step 4.
	// 2) Set vx and vy for PCG sprite's position relative to sprite origin.
	//    Mind that hardware sprites use 0,0 for their top-left.
	// 3) Call add_frm_dat with data from above.
	// 4) Erase the 16x16 image data from imgdat (set it to zero)
	// 5) Increment s_frm_offs.

	render_region(imgdat, iw, ih, sx, sy, sw, sh);

	int clip_x, clip_y;
	int last_vx = 0;
	int last_vy = 0;
	while (claim(imgdat, iw, ih, sx, sy, sw, sh, &clip_x, &clip_y))
	{
		sp_count++;

		uint8_t pcg_data[32 * 4];  // Four 8x8 tiles, row interleaved.
		const int limx = sx + sw;
		const int limy = sx + sh;
		clip_8x8_tile(imgdat, iw, clip_x, clip_y, limx, limy, &pcg_data[32 * 0]);
		clip_8x8_tile(imgdat, iw, clip_x, clip_y + 8, limx, limy, &pcg_data[32 * 1]);
		clip_8x8_tile(imgdat, iw, clip_x + 8, clip_y, limx, limy, &pcg_data[32 * 2]);
		clip_8x8_tile(imgdat, iw, clip_x + 8, clip_y + 8, limx, limy, &pcg_data[32 * 3]);

		int pt_idx = find_pcg_dat(pcg_data);
		if (pt_idx < 0)
		{
			pt_idx = s_pcg_count;
			add_pcg_dat(pcg_data);
		}

		if (mode != CONV_MODE_XOBJ) continue;

		const int vx = ((clip_x % sw) - ox);
		const int vy = ((clip_y % sh) - oy);
		add_frm_dat(vx - last_vx, vy - last_vy, pt_idx, 0);
//		render_region(imgdat, iw, ih, sx, sy, sw, sh);
		last_vx = vx;
		last_vy = vy;
	}

//	printf("Used %d sprites\n", sp_count);

	if (mode != CONV_MODE_XOBJ) return;
	add_ref_dat(sp_count, frm_offs);
}

//
// Entry point
//

int main(int argc, char **argv)
{
	if (!check_arg_sanity(argc, argv)) return 0;
	
	// User parameters.
	const char *fname = argv[1];
	const int frame_w = strtoul(argv[2], NULL, 0);
	const int frame_h = strtoul(argv[3], NULL, 0);
	const char *outname = argv[4];
	ConvMode mode = conv_mode_from_args(argc, argv);
	const ConvOrigin origin = conv_origin_from_args(argc, argv);

	// Prepare the PNG image.
	unsigned int png_w = 0;
	unsigned int png_h = 0;
	uint8_t *imgdat = load_png_data(fname, &png_w, &png_h);
	if (!imgdat) return -1;
	if (frame_w > png_w || frame_h > png_h)
	{
		printf("Frame size (%d x %d) exceed source image (%d x %d)\n",
		       frame_w, frame_h, png_w, png_h);
		goto finished;
	}

	// Choose conversion mode if auto is specified.
	// TODO: Gracefully handle < 16px frames.
	if (mode == CONV_MODE_AUTO &&
	    frame_h == PCG_TILE_PX && frame_w == PCG_TILE_PX)
	{
		mode = CONV_MODE_SP;
	}
	else
	{
		mode = CONV_MODE_XOBJ;
	}

	// Set up output handles.
	if (!init_file_handles(mode, outname)) goto finished;

	// Chop sprites out of the image data.

	s_pcg_dat = malloc(128 * 65536);  // Max 65536 PCG sprites.

	const int sprite_rows = png_h / frame_h;
	const int sprite_columns = png_w / frame_w;
	printf("%d x %d cells --> %d metasprites\n", sprite_columns, sprite_rows, sprite_columns * sprite_rows);
	for (int y = 0; y < sprite_rows; y++)
	{
		for (int x = 0; x < sprite_columns; x++)
		{
//			printf("FRM %d\n", x + y * sprite_columns + 1);
			chop_sprite(imgdat, png_w, png_h, mode, origin, x * frame_w, y * frame_h, frame_w, frame_h);
		}
	}

	if (mode == CONV_MODE_SP)
	{
		printf("Used %d PCG tiles.\n", s_pcg_count);
	}
	else
	{
		printf("Used %d PCG tiles for %d sprites.\n", s_pcg_count, s_frm_offs);
	}

	fwrite(s_pcg_dat, 128, s_pcg_count, sf_pcg_out);
	fclose(sf_pcg_out);
	if (sf_frm_out) fclose(sf_frm_out);
	if (sf_ref_out) fclose(sf_ref_out);

	free(s_pcg_dat);

finished:
	free(imgdat);

	return 0;
}
