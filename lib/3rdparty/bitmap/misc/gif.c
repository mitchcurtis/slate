/*
Compile like so:
gcc -Wall -DTEST gif.c ../bmp.c -o giftest

References:
http://www.w3.org/Graphics/GIF/spec-gif89a.txt
Nelson, M.R. : "LZW Data Compression", Dr. Dobb's Journal, October 1989.
http://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art011
http://www.matthewflickinger.com/lab/whatsinagif/bits_and_bytes.asp
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "../bmp.h"
#include "gif.h"

int gif_verbose = 0;

#pragma pack(push, 1) /* Don't use any padding */

struct gif_triplet {
    uint8_t r, g, b;
};

typedef struct {

    /* header */
    struct {
        char signature[3];
        char version[3];
    } header;

    enum {gif_87a, gif_89a} version;

    /* logical screen descriptor */
    struct {
        unsigned short width;
        unsigned short height;
        unsigned char fields;
        unsigned char background;
        unsigned char par; /* pixel aspect ratio */
    } lsd;

    GIF *gif;

} GIF_FILE;

/* GIF Graphic Control Extension */
typedef struct {
    unsigned char block_size;
    unsigned char fields;
    unsigned short delay;
    unsigned char trans_index;
    unsigned char terminator;
} GIF_GCE;

/* GIF Image Descriptor */
typedef struct {
    unsigned char separator;
    unsigned short left;
    unsigned short top;
    unsigned short width;
    unsigned short height;
    unsigned char fields;
} GIF_ID;

typedef struct {
    unsigned char block_size;
    char app_id[8];
    char auth_code[3];
} GIF_APP_EXT;

typedef struct  {
	uint8_t		len;
	uint8_t		index;
	uint16_t	reps;
} NAB_BLOCK;

typedef struct {
    unsigned char block_size;
    unsigned short grid_left;
    unsigned short grid_top;
    unsigned short grid_width;
    unsigned short grid_height;
    unsigned char text_fg;
    unsigned char text_bg;
} GIF_TXT_EXT;

#pragma pack(pop)

/* Already defined in bmp.c for the GIF and PCX code, but duplicated here because I
	need to reconsider the API */
static int cnt_comp_mask(const void*ap, const void*bp);
static int count_colors_build_palette(Bitmap *b, struct gif_triplet rgb[256]);
static int bsrch_palette_lookup(struct gif_triplet rgb[], int c, int imin, int imax);
static int comp_rgb(const void *ap, const void *bp);

static int gif_read_image(FILE *fp, GIF_FILE *gif, struct gif_triplet *ct, int sct);
static int gif_read_tbid(FILE *fp, GIF_FILE *gif, GIF_ID *gif_id, GIF_GCE *gce, struct gif_triplet *ct, int sct);
static unsigned char *gif_data_sub_blocks(FILE *fp, int *r_tsize);
static unsigned char *lzw_decode_bytes(unsigned char *bytes, int data_len, int code_size, int *out_len);

static void output(FILE *outfile, const char *fmt, ...) {
	if(gif_verbose) {
		va_list arg;
		va_start(arg, fmt);
		vfprintf(outfile, fmt, arg);
		va_end(arg);
	}
}

static GIF *gif_load_fp(FILE *fp) {
    GIF_FILE file;

    /* From the packed fields in the logical screen descriptor */
    int gct, col_res, sort_flag, sgct;
    float aspect_ratio;

    struct gif_triplet *palette = NULL;

    unsigned char trailer;

    file.gif = NULL;

    /* Section 17. Header. */
    if(fread(&file.header, sizeof file.header, 1, fp) != 1) {
        output(stderr, "error: unable to read header\n");
        return NULL;
    }
    if(memcmp(file.header.signature, "GIF", 3)){
        output(stderr, "error: not a GIF\n");
        return NULL;
    }
    if(!memcmp(file.header.version, "87a", 3)){
        output(stdout, "GIF 87a\n");
        file.version = gif_87a;
    } else if(!memcmp(file.header.version, "89a", 3)){
        output(stdout, "GIF 89a\n");
        file.version = gif_89a;
    } else {
        output(stderr, "error: invalid version number\n");
        return NULL;
    }

    /* Section 18. Logical Screen Descriptor. */

    /* Ugh, the compiler I used added a padding byte */
    assert(sizeof file.lsd == 7);
    assert(sizeof *palette == 3);

    if(fread(&file.lsd, sizeof file.lsd, 1, fp) != 1) {
        output(stderr, "error: unable to read Logical Screen Descriptor\n");
        return NULL;
    }

    gct = !!(file.lsd.fields & 0x80);
    col_res = ((file.lsd.fields >> 4) & 0x07) + 1;
    sort_flag = !!(file.lsd.fields & 0x08);
    sgct = file.lsd.fields & 0x07;

    if(gct) {
        /* raise 2 to the power of [sgct+1] */
        sgct = 1 << (sgct + 1);
    }

    if(file.lsd.par == 0) {
        aspect_ratio = 0.0f;
    } else {
        aspect_ratio = ((float)file.lsd.par + 15.0f)/64.0f;
    }

	if(gif_verbose) {
		output(stdout, "Width .......................: %u\n", file.lsd.width);
		output(stdout, "Height ......................: %u\n", file.lsd.height);
		output(stdout, "Fields ......................: 0x%X\n", file.lsd.fields);
		output(stdout, "  Global color table ..: %d\n", gct);
		output(stdout, "  Color Resolution ....: %d\n", col_res);
		output(stdout, "  Sort Flag ...........: %d\n", sort_flag);
		output(stdout, "  Size of GCT .........: %d\n", sgct);
		output(stdout, "Background ..................: %u\n", file.lsd.background);
		output(stdout, "Px Aspect Ratio .............: %u\n", file.lsd.par);
		output(stdout, "  Calculated ..........: %.4f\n", aspect_ratio);
	}
    file.gif = gif_create(file.lsd.width, file.lsd.height);

    if(gct) {
        /* Section 19. Global Color Table. */
        struct gif_triplet *bg;
        palette = calloc(sgct, sizeof *palette);

        if(fread(palette, sizeof *palette, sgct, fp) != sgct) {
            output(stderr, "error: unable to read Global Color Table\n");
            free(palette);
            return NULL;
        }

        output(stdout, "Global Color Table: %d entries\n", sgct);
		if(gif_verbose > 1) {
			int i;
			for(i = 0; i < sgct; i++) {
				output(stdout, " %3d: %02X %02X %02X\n", i, palette[i].r, palette[i].g, palette[i].b);
			}
		}
        /* Set the Bitmap's color to the background color.*/
        bg = &palette[file.lsd.background];
        file.gif->background = bm_rgb(bg->r, bg->g, bg->b);
    } else {
        /* what? */
		file.gif->background = 0;
        palette = NULL;
    }

    for(;;) {
        long pos = ftell(fp);
        if(!gif_read_image(fp, &file, palette, sgct)) {
            fseek(fp, pos, SEEK_SET);
            break;
        }
    }

    if(palette)
        free(palette);

    /* Section 27. Trailer. */
    if((fread(&trailer, 1, 1, fp) != 1) || trailer != 0x3B) {
        output(stderr, "error: trailer is not 0x3B\n");
        gif_free(file.gif);
        return NULL;
    }
    output(stdout, "Trailer: %02X\n", trailer);

    return file.gif;
}

static int gif_read_extension(FILE *fp, GIF_FILE *file, GIF_GCE *gce) {

    unsigned char introducer, label;

    if((fread(&introducer, 1, 1, fp) != 1) || introducer != 0x21) {
        return 0;
    }

    if(fread(&label, 1, 1, fp) != 1) {
        return 0;
    }

    output(stdout, "# Introducer ..........: 0x%02X\n", introducer);
    output(stdout, "# Label ...............: 0x%02X\n", label);

    if(label == 0xF9) {
        /* 23. Graphic Control Extension. */
        if(fread(gce, sizeof *gce, 1, fp) != 1) {
            output(stderr, "warning: unable to read Graphic Control Extension\n");
            return 0;
        }
		if(gif_verbose) {
			output(stdout, "Graphic Control Extension:\n");
			output(stdout, "  Terminator ..........: 0x%02X\n", gce->terminator);
			output(stdout, "  Block Size ..........: %d\n", gce->block_size);
			output(stdout, "  Fields ..............: 0x%02X\n", gce->fields);
			output(stdout, "    Dispose ......: %d\n", (gce->fields >> 2) & 0x07);
			output(stdout, "    User Input ...: %d\n", !!(gce->fields & 0x02));
			output(stdout, "    Transparent ..: %d\n", gce->fields & 0x01);
			output(stdout, "  Delay ...............: %u\n", gce->delay);
			output(stdout, "  Transparent Index ...: %d\n", gce->trans_index);
		}
    } else if(label == 0xFE) {
        /* Section 24. Comment Extension. */
        int len;
        unsigned char *bytes = gif_data_sub_blocks(fp, &len);
        output(stdout, "Comment Extension: (%d bytes)\n  '%s'\n", len, bytes);
    } else if(label == 0x01) {
        /* Section 25. Plain Text Extension. */
        GIF_TXT_EXT te;
        int len;
        unsigned char *bytes;
        if(fread(&te, sizeof te, 1, fp) != 1) {
            output(stderr, "warning: unable to read Text Extension\n");
            return 0;
        }
        bytes = gif_data_sub_blocks(fp, &len);
		(void)bytes;
        output(stdout, "Text Extension: (%d bytes)\n", len);
    } else if(label == 0xFF) {
        /* Section 26. Application Extension. */
        GIF_APP_EXT ae;
        int len;
        unsigned char *bytes;
        if(fread(&ae, sizeof ae, 1, fp) != 1) {
            output(stderr, "warning: unable to read Application Extension\n");
            return 0;
        }
		char app_id[9];
		char auth_code[4];
        memcpy(app_id, ae.app_id, 8); app_id[8] = '\0';
        memcpy(auth_code, ae.auth_code, 3); auth_code[3] = '\0';
		bytes = gif_data_sub_blocks(fp, &len);
        output(stdout, "Application Extension: (%d bytes) app_id:'%s' auth_code:'%s'\n", len, app_id, auth_code);
		if(!strcmp(app_id, "NETSCAPE")) { // && !strcmp(auth_code, "2.0")) {
			/*
			https://en.wikipedia.org/wiki/GIF#Animated_GIF
			http://www.vurdalakov.net/misc/gif/netscape-looping-application-extension
			*/
			int index = bytes[0];
			assert(index == 1);
			file->gif->repetitions = (bytes[2] << 8) | bytes[1];
			output(stdout, "Got a NETSCAPE application extension: index:%d; repetitions:%d\n", index, file->gif->repetitions);
		}
    } else {
        output(stdout, "error: unknown label 0x%02X\n", label);
    }
    return 1;
}

/* Section 20. Image Descriptor. */
static int gif_read_image(FILE *fp, GIF_FILE *file, struct gif_triplet *ct, int sct) {
    GIF_GCE gce;
    GIF_ID gif_id;

    /* Packed fields in the Image Descriptor */
    int lct, intl, sort, slct;

    memset(&gce, 0, sizeof gce);

    if(file->version >= gif_89a) {
        for(;;) {
            long pos = ftell(fp);
            if(!gif_read_extension(fp, file, &gce)) {
                fseek(fp, pos, SEEK_SET);
                break;
            }
        }
    }

    if(fread(&gif_id, sizeof gif_id, 1, fp) != 1) {
        return 0; /* no more blocks to read */
    }

    if(gif_id.separator != 0x2C) {
        output(stderr, "error: block is not an image descriptor (0x%02X)\n", gif_id.separator);
        return 0;
    }

    lct = !!(gif_id.fields & 0x80);
    intl = !!(gif_id.fields & 0x40);
    sort = !!(gif_id.fields & 0x20);
    slct = gif_id.fields & 0x07;
    if(lct) {
        /* Section 21. Local Color Table. */

        /* raise 2 to the power of [slct+1] */
        slct = 1 << (slct + 1);

        ct = calloc(slct, sizeof *ct);

        if(fread(ct, sizeof *ct, slct, fp) != slct) {
            output(stderr, "error: unable to read local color table\n");
            free(ct);
            return 0;
        }

        output(stdout, "Local Color Table: %d entries\n", slct);
		if(gif_verbose > 1) {
			int i;
			for(i = 0; i < slct; i++) {
				output(stdout, " %3d: %02X %02X %02X\n", i, ct[i].r, ct[i].g, ct[i].b);
			}
		}

        sct = slct;
    }

	if(gif_verbose) {
		output(stdout, "Image Descriptor:\n");
		output(stdout, "  Left ................: %d\n", gif_id.left);
		output(stdout, "  Top .................: %d\n", gif_id.top);
		output(stdout, "  Width ...............: %d\n", gif_id.width);
		output(stdout, "  Height ..............: %d\n", gif_id.height);
		output(stdout, "  Fields ..............: 0x%02X\n", gif_id.fields);
		output(stdout, "    LCT ..........: %d\n", lct);
		output(stdout, "    Interlace ....: %d\n", intl);
		output(stdout, "    Sort .........: %d\n", sort);
		output(stdout, "    Size of LCT ..: %d\n", slct);
	}
    if(!gif_read_tbid(fp, file, &gif_id, &gce, ct, sct)) {
        return 0; /* what? */
    }

    if(lct) {
        free(ct);
    }

    return 1;
}

/* Section 15. Data Sub-blocks. */
static unsigned char *gif_data_sub_blocks(FILE *fp, int *r_tsize) {
    unsigned char *buffer = NULL, *pos, size;
    int tsize = 0;

    if(r_tsize)
        *r_tsize = 0;

    if(fread(&size, 1, 1, fp) != 1) {
        return NULL;
    }
    buffer = realloc(buffer, 1);

    while(size > 0) {
        if(gif_verbose > 2) {
			output(stdout, "  Size ................: %d (%d)\n", size, tsize);
		}
        buffer = realloc(buffer, tsize + size + 1);
        pos = buffer + tsize;

        if(fread(pos, sizeof *pos, size, fp) != size) {
            free(buffer);
            return NULL;
        }

        tsize += size;
        if(fread(&size, 1, 1, fp) != 1) {
            free(buffer);
            return NULL;
        }
    }

    if(r_tsize)
        *r_tsize = tsize;
    buffer[tsize] = '\0';
    return buffer;
}

/* Section 22. Table Based Image Data. */
static int gif_read_tbid(FILE *fp, GIF_FILE *file, GIF_ID *gif_id, GIF_GCE *gce, struct gif_triplet *ct, int sct) {
    int len, rv = 1;
    unsigned char *bytes, min_code_size;

    if(fread(&min_code_size, 1, 1, fp) != 1) {
        return 0;
    }

    output(stdout, "Table Based Image Data:\n");
    output(stdout, "  Minimum Code Size ...: %d\n", min_code_size);

    bytes = gif_data_sub_blocks(fp, &len);
    if(bytes && len > 0) {
        int i, outlen, x, y;
        /* Packed fields in the Image Descriptor */
        int lct, intl, sort, slct;

        /* Packed fields in the Graphic Control Extension */
        int dispose = 0, user_in = 0, trans_flag = 0;

        lct = !!(gif_id->fields & 0x80);
        intl = !!(gif_id->fields & 0x40);
        sort = !!(gif_id->fields & 0x20);
        slct = gif_id->fields & 0x07;

		(void)lct; (void)sort; (void)slct;

        if(gce->block_size) {
            /* gce->block_size will be 4 if the GCE is present, 0 otherwise */
            dispose = (gce->fields >> 2) & 0x07;
            user_in = !!(gce->fields & 0x02);
			(void)user_in;
            trans_flag = gce->fields & 0x01;
        }

        if(gif_id->top + gif_id->height > file->gif->h ||
            gif_id->left + gif_id->width > file->gif->w) {
            output(stderr, "error: this image descriptor doesn't fall within the bounds of the image");
            return 0;
        }

        output(stdout, "Data block: %d bytes\n", len);
		if(gif_verbose > 1) {
			for(i = 0; i < len; i++) {
				output(stdout, "%02X ", bytes[i]);
			}
			output(stdout, "\n");
		}

		unsigned int background = 0;
		if(trans_flag) {
			/* Mmmm, my bitmap module won't be able to handle
				situations where different image blocks in the
				GIF has different transparent colors */
			struct gif_triplet *bg = &ct[gce->trans_index];
			background = bm_rgb(bg->r, bg->g, bg->b);
		}

		if(dispose == 2) {
			GIF_FRAME *frame = gif_new_frame(file->gif);
			Bitmap *b = frame->image;
			bm_set_color(b, background);
			bm_clear(b);
			frame->delay = gce->delay;
			frame->trans = gce->fields & 0x01;
        } else if(dispose != 3) {
            /* dispose = 0 or 1; if dispose is 3, we leave the new image */
            GIF_FRAME *frame;
			if(file->gif->n) {
				frame = &file->gif->frames[file->gif->n - 1];
				frame = gif_add_frame(file->gif, frame->image);
			} else {
				frame = gif_new_frame(file->gif);
			}
			frame->delay = gce->delay;
			frame->trans = gce->fields & 0x01;
			
			Bitmap *b = frame->image;
			unsigned char *decoded = lzw_decode_bytes(bytes, len, min_code_size, &outlen);
            if(decoded) {
				if(gif_verbose > 1) {
					for(i = 0; i < outlen; i++) {
						output(stdout, "%02X ", decoded[i]);
					}
					output(stdout, "\n");
				}
                if(outlen != gif_id->width * gif_id->height) {
                    /* Shouldn't happen unless the file is corrupt */
                    output(stderr, "error: %d decoded bytes does not fit dimensions %d x %d pixels\n", outlen, gif_id->width, gif_id->height);
                    rv = 0;
                } else {
                    /* Vars for interlacing: */
                    int grp = 1, /* Group we're in */
                        inty = 0, /* Y we're currently at */
                        inti = 8, /* amount by which we should increment inty */
                        truey; /* True Y, taking interlacing and the image descriptor into account */
                    output(stdout, "%d decoded bytes; %d x %d pixels\n", outlen, gif_id->width, gif_id->height);
                    for(i = 0, y = 0; y < gif_id->height && rv; y++) {
                        /* Appendix E. Interlaced Images. */
                        if(intl) {
                            truey = inty + gif_id->top;
                            inty += inti;
                            if(inty >= gif_id->height) {
                                switch(++grp) {
                                    case 2: inti = 8; inty = 4; break;
                                    case 3: inti = 4; inty = 2; break;
                                    case 4: inti = 2; inty = 1;break;
                                }
                            }
                        } else {
                            truey = y + gif_id->top;
                        }
                        assert(truey >= 0 && truey < file->gif->h);
                        for(x = 0; x < gif_id->width && rv; x++, i++) {
                            int c = decoded[i];
                            if(c < sct) {
                                struct gif_triplet *rgb = &ct[c];
                                assert(x + gif_id->left >= 0 && x + gif_id->left < file->gif->w);
                                if(!trans_flag || c != gce->trans_index) {
                                    bm_set(b, x + gif_id->left, truey, bm_rgb(rgb->r, rgb->g, rgb->b));
                                }
                            } else {
                                /* Decode error */
                                rv = 0;
                            }
                        }
                    }
                }
                free(decoded);
            }
        }
        free(bytes);
    }
    return rv;
}

typedef struct {
    int prev;
    int code;
} gif_dict;

static int lzw_read_code(unsigned char bytes[], int bits, int *pos) {
    int i, bi, code = 0;
    assert(pos);
    for(i = *pos, bi=1; i < *pos + bits; i++, bi <<=1) {
        int byte = i >> 3;
        int bit = i & 0x07;
        if(bytes[byte] & (1 << bit))
            code |= bi;
    }
    *pos = i;
    return code;
}

static unsigned char *lzw_decode_bytes(unsigned char *bytes, int data_len, int code_size, int *out_len) {
    unsigned char *out = NULL;
    int out_size = 32;
    int outp = 0;

    int base_size = code_size;

    int pos = 0, code, old = -1;

    /* Clear and end of stream codes */
    int clr = 1 << code_size;
    int end = clr + 1;

    /* Dictionary */
    int di, dict_size = 1 << (code_size + 1);
    gif_dict *dict = realloc(NULL, dict_size * sizeof *dict);

    /* Stack so we don't need to recurse down the dictionary */
    int stack_size = 2;
    unsigned char *stack = realloc(NULL, stack_size);
    int sp = 0;
    int sym = -1, ptr;

    *out_len = 0;
    out = realloc(NULL, out_size);

    /* Initialize the dictionary */
    for(di = 0; di < dict_size; di++) {
        dict[di].prev = -1;
        dict[di].code = di;
    }
    di = end + 1;

    code = lzw_read_code(bytes, code_size + 1, &pos);
    while((pos >> 3) <= data_len + 1) {
        if(code == clr) {
            code_size = base_size;
            dict_size = 1 << (code_size + 1);
            di = end + 1;
            code = lzw_read_code(bytes, code_size + 1, &pos);
            old = -1;
            continue;
        } else if(code == end) {
            break;
        }

        if(code > di) {
            /* Shouldn't happen, unless file corrupted */
            output(stderr, "error: code (%02Xh) is outside dictionary (%02Xh); code size: %d\n", code, di, code_size);
            free(out);
            return NULL;
        }

        if(code == di) {
            /* Code is not in the table */
            ptr = old;
            stack[sp++] = sym;
        } else {
            /* Code is in the table */
            ptr = code;
        }

        /* Walk down the dictionary and push the codes onto a stack */
        while(ptr >= 0) {
            stack[sp++] = dict[ptr].code;
            if(sp == stack_size) {
                stack_size <<= 1;
                stack = realloc(stack, stack_size);
            }
            ptr = dict[ptr].prev;
        }
        sym = stack[sp-1];

        /* Output the decoded bytes */
        while(sp > 0) {
            out[outp++] = stack[--sp];
            if(outp == out_size) {
                out_size <<= 1;
                out = realloc(out, out_size);
            }
        }

        /* update the dictionary */
        if(old >= 0) {
            if(di < dict_size) {
                dict[di].prev = old;
                dict[di].code = sym;
                di++;
            }
            /* Resize the dictionary? */
            if(di == dict_size && code_size < 11) {
                code_size++;
                dict_size = 1 << (code_size + 1);
                dict = realloc(dict, dict_size * sizeof *dict);
            }
        }

        old = code;
        code = lzw_read_code(bytes, code_size + 1, &pos);
    }
    free(stack);
    free(dict);

    *out_len = outp;
    return out;
}

static void lzw_emit_code(unsigned char **buffer, int *buf_size, int *pos, int c, int bits) {
    int i, m;
    for(i = *pos, m = 1; i < *pos + bits; i++, m <<= 1) {
        int byte = i >> 3;
        int bit = i & 0x07;
        if(!bit) {
            if(byte == *buf_size) {
                *buf_size <<= 1;
                *buffer = realloc(*buffer, *buf_size);
            }
            (*buffer)[byte] = 0x00;
        }
        if(c & m)
            (*buffer)[byte] |= (1 << bit);
    }
    *pos += bits;
}

static unsigned char *lzw_encode_bytes(unsigned char *bytes, int data_len, int code_size, int *out_len) {
    int base_size = code_size;

    /* Clear and end of stream codes */
    int clr = 1 << code_size;
    int end = clr + 1;

    /* dictionary */
    int i, di, dict_size = 1 << (code_size + 1);
    gif_dict *dict = realloc(NULL, dict_size * sizeof *dict);

    int buf_size = 4;
    int pos = 0;
    unsigned char *buffer = realloc(NULL, buf_size);

    *out_len = 0;

    /* Initialize the dictionary */
    for(di = 0; di < dict_size; di++) {
        dict[di].prev = -1;
        dict[di].code = di;
    }
    di = end+1;

    dict[clr].prev = -1;
    dict[clr].code = -1;
    dict[end].prev = -1;
    dict[end].code = -1;

    int ii = 0;
    int string = -1;
    int prev = clr;

    lzw_emit_code(&buffer, &buf_size, &pos, clr, code_size + 1);

    for(ii = 0; ii < data_len; ii++) {
        int character;
reread:
        character = bytes[ii];

        /* Find it in the dictionary; If the entry is in the dict, it can't be
        before dict[string], therefore we can eliminate the first couple of entries. */
        int res = -1;
        for(i = string>0?string:0; i < di; i++) {
            if(dict[i].prev == string && dict[i].code == character) {
                res = i;
                break;
            }
        }

        if(res >= 0) {
            /* Found */
            string = res;
            prev = res;
        } else {
            /* Not found */
            lzw_emit_code(&buffer, &buf_size, &pos, prev, code_size + 1);

            /* update the dictionary */
            if(di == dict_size) {
                /* Resize the dictionary */
                if(code_size < 11) {
                    code_size++;
                    dict_size = 1 << (code_size + 1);
                    dict = realloc(dict, dict_size * sizeof *dict);
                } else {
                    /* lzw_emit_code a clear code */
                    lzw_emit_code(&buffer, &buf_size, &pos, clr,code_size + 1);
                    code_size = base_size;
                    dict_size = 1 << (code_size + 1);
                    di = end + 1;
                    string = -1;
                    prev = clr;
                    goto reread;
                }
            }

            dict[di].prev = string;
            dict[di].code = character;
            di++;

            string = character;
            prev = character;
        }
    }

    lzw_emit_code(&buffer, &buf_size, &pos, prev,code_size + 1);
    lzw_emit_code(&buffer, &buf_size, &pos, end,code_size + 1);

    /* Total length */
    int tlen = (pos >> 3);
    if(pos & 0x07) tlen++;
    *out_len = tlen;

    return buffer;
}

GIF *gif_load(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    GIF *gif;
    if(!fp) {
        output(stderr, "error: unable to open %s\n", filename);
        return NULL;
    }
    gif = gif_load_fp(fp);

    fclose(fp);
    return gif;
}

static int gif_save_fp(GIF *g, FILE *f) {
    GIF_FILE file;
    GIF_GCE gce;
    GIF_ID gif_id;
    int nc, sgct, bg;
    struct gif_triplet gct[256];
    unsigned char code_size = 0x08;
	Bitmap *b = g->n > 0 ? g->frames[0].image : NULL;

	if(!b) return 0;

    memcpy(file.header.signature, "GIF", 3);
    memcpy(file.header.version, "89a", 3);
    file.version = gif_89a;
    file.lsd.width = g->w;
    file.lsd.height = g->h;
    file.lsd.background = 0;
    file.lsd.par = 0;

    /* Using global color table, color resolution = 8-bits */
    file.lsd.fields = 0xF0;

    nc = count_colors_build_palette(b, gct);
	unsigned int palette[256], q;

    if(nc < 0) {
        /* Too many colors */
        sgct = 256;
        file.lsd.fields |= 0x07;
        for(nc = 0; nc < 256; nc++) {
			/* FIXME: Octree color quantization
			My color quantization method is to just choose random pixels, 
			and hope for the best. */
            int c = bm_get(b, rand()%b->w, rand()%b->h);
            gct[nc].r = (c >> 16) & 0xFF;
            gct[nc].g = (c >> 8) & 0xFF;
            gct[nc].b = (c >> 0) & 0xFF;
        }
        qsort(gct, nc, sizeof gct[0], comp_rgb);
    } else {
        if(nc > 128) {
            sgct = 256;
            file.lsd.fields |= 0x07;
        } else if(nc > 64) {
            sgct = 128;
            file.lsd.fields |= 0x06;
            code_size = 7;
        } else if(nc > 32) {
            sgct = 64;
            file.lsd.fields |= 0x05;
            code_size = 6;
        } else if(nc > 16) {
            sgct = 32;
            file.lsd.fields |= 0x04;
            code_size = 5;
        } else if(nc > 8) {
            sgct = 16;
            file.lsd.fields |= 0x03;
            code_size = 4;
        } else {
            sgct = 8;
            file.lsd.fields |= 0x02;
            code_size = 3;
        }
    }
	for(q = 0; q < nc; q++) {
		palette[q] = (gct[q].r << 16) | (gct[q].g << 8) | gct[q].b;
	}

    /* See if we can find the background color in the palette */
    bg = b->color & 0x00FFFFFF;
    bg = bsrch_palette_lookup(gct, bg, 0, nc - 1);
    if(bg >= 0) {
        file.lsd.background = bg;
    }

    if(fwrite(&file.header, sizeof file.header, 1, f) != 1) {
        output(stderr, "error: unable to write header.\n");
        return 0;
    }

    if(fwrite(&file.lsd, sizeof file.lsd, 1, f) != 1) {
        output(stderr, "error: unable to write logical screen descriptor.\n");
        return 0;
    }

    if(fwrite(gct, sizeof *gct, sgct, f) != sgct) {
        output(stderr, "error: unable to write global color table.\n");
        return 0;
    }

	if(g->repetitions != 1) {
		/* Netscape Application Block for looping animations
		https://en.wikipedia.org/wiki/GIF#Animated_GIF
		http://www.vurdalakov.net/misc/gif/netscape-looping-application-extension
		*/
		GIF_APP_EXT ae;
		NAB_BLOCK nab;

		fputc(0x21, f);
		fputc(0xFF, f);

		ae.block_size = 0x0B;
		memcpy(ae.app_id, "NETSCAPE", 8);
		memcpy(ae.auth_code, "2.0", 3);

		nab.len = 3;
		nab.index = 1;
		nab.reps = g->repetitions;
		assert(sizeof nab == 4);

		if(fwrite(&ae, sizeof ae, 1, f) != 1 || fwrite(&nab, sizeof nab, 1, f) != 1) {
			output(stderr, "error: unable to write Netscape Application Block.\n");
			return 0;
		}
		fputc(0x00, f); /* terminating block */
	}

	unsigned char *pixels = malloc(g->w * g->h);
	int fi;
	for(fi = 0; fi < g->n; fi++) {

		b = g->frames[fi].image;
		assert(b->w == g->w && b->h == g->h);

        bm_reduce_palette(b, palette, nc);

		/* Map the pixels in the image to their palette indices */
		int x, y, p = 0;
		for(y = 0; y < b->h; y++) {
			for(x = 0; x < b->w; x++) {
				int i;
				int c = bm_get(b, x, y);
				i = bsrch_palette_lookup(gct, c, 0, nc - 1);
				/* At this point in time, the color MUST be in the palette */
				assert(i >= 0);
				assert(i < sgct);
				pixels[p++] = i;
			}
		}
		assert(p == b->w * b->h);

		gce.block_size = 4;
		gce.fields = 0x04;
		gce.delay = g->frames[fi].delay;
		if(g->frames[fi].trans && bg >= 0) {
			gce.fields |= 0x01;
			gce.trans_index = bg;
		} else {
			gce.trans_index = 0;
		}
		gce.terminator = 0x00;

		fputc(0x21, f);
		fputc(0xF9, f);
		if(fwrite(&gce, sizeof gce, 1, f) != 1) {
			output(stderr, "error: unable to write graphic control extension.\n");
			return 0;
		}

		gif_id.separator = 0x2C;
		gif_id.left = 0x00;
		gif_id.top = 0x00;
		gif_id.width = b->w;
		gif_id.height = b->h;
		/* Not using local color table or interlacing */
		gif_id.fields = 0;
		if(fwrite(&gif_id, sizeof gif_id, 1, f) != 1) {
			output(stderr, "error: unable to write image descriptor.\n");
			return 0;
		}

		fputc(code_size, f);

		/* Perform the LZW compression */
		int len;
		unsigned char *bytes = lzw_encode_bytes(pixels, b->w * b->h, code_size, &len);

		/* Write out the data sub-blocks */
		for(p = 0; p < len; p++) {
			if(p % 0xFF == 0) {
				/* beginning of a new block; lzw_emit_code the length byte */
				if(len - p >= 0xFF) {
					fputc(0xFF, f);
				} else {
					fputc(len - p, f);
				}
			}
			fputc(bytes[p], f);
		}
		free(bytes);

		fputc(0x00, f); /* terminating block */
	}
	free(pixels);

    fputc(0x3B, f); /* trailer byte */

    return 1;
}

int gif_save(GIF *g, const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if(!fp) {
        output(stderr, "error: unable to open %s\n", filename);
        return 0;
    }
    gif_save_fp(g, fp);
    fclose(fp);
    return 1;
}

GIF *gif_create(int w, int h) {
	GIF *gif = malloc(sizeof *gif);
	gif->w = w;
	gif->h = h;
	gif->background = 0;
	gif->repetitions = 10;
	gif->a = 8;
	gif->n = 0;
	gif->frames = calloc(gif->a, sizeof *gif->frames);
	return gif;
}

void gif_free(GIF *gif) {
	int i;
	for(i = 0; i < gif->n; i++) {
		bm_free(gif->frames[i].image);
	}
	free(gif->frames);
	free(gif);
}

GIF_FRAME *gif_add_frame(GIF *g, Bitmap *b) {
	if(b->w != g->w || b->h != g->h) {
		if(g->w > b->w || g->h > b->h) {
			b = bm_resample_blin(b, g->w, g->h);
		} else {
			b = bm_resample_bcub(b, g->w, g->h);
		}
	} else {
		b = bm_copy(b);
	}
	bm_set_color(b, g->background);
	if(g->n == g->a) {
		g->a <<= 1;
		g->frames = realloc(g->frames, g->a * sizeof *g->frames);
	}
	assert(g->n < g->a);
	memset(&g->frames[g->n], 0, sizeof g->frames[g->n]);
	g->frames[g->n].image = b;
	g->frames[g->n].delay = 2;
	return &g->frames[g->n++];
}

GIF_FRAME *gif_new_frame(GIF *g) {
	Bitmap *b = bm_create(g->w, g->h);
	bm_set_color(b, g->background);
	bm_clear(b);
	if(g->n == g->a) {
		g->a <<= 1;
		g->frames = realloc(g->frames, g->a * sizeof *g->frames);
	}
	assert(g->n < g->a);
	memset(&g->frames[g->n], 0, sizeof g->frames[g->n]);
	g->frames[g->n].image = b;
	g->frames[g->n].delay = 2;
	return &g->frames[g->n++];
}

#if 1
/*****
FIXME: These functions are already defined as static in bmp.c.
I'd like to modify the API at some stage to expose the functionality
via bmp.h, but the function prototypes would need some thought.
Ideally, I should have a bm_quantize() function that takes care of
quantizing the image and creating a palette.
Rosettacode has a nice color quantization implementation based on octrees.
http://rosettacode.org/wiki/Color_quantization#C
*****/
static int cnt_comp_mask(const void*ap, const void*bp) {
    int a = *(int*)ap, b = *(int*)bp;
    return (a & 0x00FFFFFF) - (b & 0x00FFFFFF);
}
/* Variation on bm_count_colors() that builds an 8-bit palette while it is counting.
 * It returns -1 in case there are more than 256 colours in the palette, meaning the
 * image will have to be quantized first.
 * It also ignores the alpha values of the pixels.
 */
static int count_colors_build_palette(Bitmap *b, struct gif_triplet rgb[256]) {
    int count = 1, i, c;
    int npx = b->w * b->h;
    int *sort = malloc(npx * sizeof *sort);
    memcpy(sort, b->data, npx * sizeof *sort);
    qsort(sort, npx, sizeof(int), cnt_comp_mask);
    c = sort[0] & 0x00FFFFFF;
    rgb[0].r = (c >> 16) & 0xFF;
    rgb[0].g = (c >> 8) & 0xFF;
    rgb[0].b = (c >> 0) & 0xFF;
    for(i = 1; i < npx; i++){
        c = sort[i] & 0x00FFFFFF;
        if(c != (sort[i-1]& 0x00FFFFFF)) {
            if(count == 256) {
                return -1;
            }
            rgb[count].r = (c >> 16) & 0xFF;
            rgb[count].g = (c >> 8) & 0xFF;
            rgb[count].b = (c >> 0) & 0xFF;
            count++;
        }
    }
    free(sort);
    return count;
}

/* Uses a binary search to find the index of a colour in a palette.
It almost goes without saying that the palette must be sorted */
static int bsrch_palette_lookup(struct gif_triplet rgb[], int c, int imin, int imax) {
    c &= 0x00FFFFFF; /* Ignore the alpha value */
    while(imax >= imin) {
        int imid = (imin + imax) >> 1;
        assert(imid <= 255);
        int c2 = (rgb[imid].r << 16) | (rgb[imid].g << 8) | rgb[imid].b;
        if(c == c2)
            return imid;
        else if(c2 < c)
            imin = imid + 1;
        else
            imax = imid - 1;
    }
    return -1;
}

/* Comparison function for sorting an array of rgb_triplets with qsort() */
static int comp_rgb(const void *ap, const void *bp) {
    const struct gif_triplet *ta = ap, *tb = bp;
    int a = (ta->r << 16) | (ta->g << 8) | ta->b;
    int b = (tb->r << 16) | (tb->g << 8) | tb->b;
    return a - b;
}
#endif

/* Test program main() function */
#ifdef TEST
int main(int argc, char *argv[]) {

	/* FIXME: The color quantization shouldn't depend on rand() :( */
    srand(time(NULL));

	gif_verbose = 1;

    if(argc < 2) {
        fprintf(stderr, "usage: %s file.gif\n", argv[0]);
        return 1;
    }
    assert(sizeof(unsigned short) == 2);

	GIF *gif = gif_load(argv[1]);
    if(gif) {
        char buffer[50];
		int i;
		for(i = 0; i < gif->n; i++) {
			sprintf(buffer, "frame%03d.bmp", i);
			printf("Saving %s...\n", buffer);
			bm_save(gif->frames[i].image, buffer);
		}
		printf("Saving GIF output...\n");
		if(!gif_save(gif, "gifout.gif")) {
            fprintf(stderr, "error: Unable to save GIF\n");
            return 1;
        }

        gif_free(gif);
    } else {
		fprintf(stderr, "Couldn't read GIF\n");
		return 1;
	}
    return 0;
}
#endif
