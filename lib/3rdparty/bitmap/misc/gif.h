/**
 * gif.c/gif.h
 * ===========
 *
 * Utility for programmatically supporting animated GIFs.
 *
 * References:
 * * https://www.w3.org/Graphics/GIF/spec-gif89a.txt
 * * https://en.wikipedia.org/wiki/GIF
 * * http://www.vurdalakov.net/misc/gif/netscape-looping-application-extension
 */

/**
 * ## Structures
 *
 * ### `struct GIF_FRAME`
 * Structure for individual animation frame.
 *
 * Add frames to the GIF through `gif_add_frame()` or
 * `gif_new_frame()`. Those functions create a copy that is scaled to match the
 * size of the GIF as specified in `gif_create()`.
 *
 * It has these members:
 * * `Bitmap *image` - The image of the frame.
 * * `unsigned char delay` - the time to display the frame, in 100th's of a second
 *   (i.e. 50 means 500ms)
 * * `trans` - Set to true if the frame should have a transparent component. The
 *   `image`'s color (see `bm_set_color()`) will determine the transparent color.
 */
typedef struct GIF_FRAME {
    Bitmap *image;
    unsigned char delay;
    unsigned char trans;
} GIF_FRAME;

/**
 * ### `struct GIF`
 * Structure containing the GIF animation.
 *
 * It has these members:
 * * `w` and `h` - The width and height in pixels of the GIF.
 * * `background` - The background color that will be used for transparency.
 * * `repetitions` - The number of repetitions of the animation, defaults to 1.
 *    set to 0 for infinite looping.
 * * `default_delay` - default delay of each frame, in 100th's of a second.
 *    The `GIF_FRAME.delay` member can be used to control the delay of each
 *    frame individually.
 * * `frames` - The internal array of `GIF_FRAME`s for storing the
 *    individual frames.
 * * `palette` and `palette_size` - The palette of the GIF. If none
 *    is specified, a palette will be selected from the first frame.
 *    See `gif_set_palette()` for more information.
 * * `n` - The number of frames.
 * * `a` - The number of allocated frames. `frames` is resized through
 *   `realloc()` as necessary.
 */
typedef struct GIF {
    int w, h;
    unsigned int background;
    int repetitions;

    unsigned char default_delay;

    unsigned int *palette;
    unsigned int palette_size;

    /* The actual frames */
    GIF_FRAME *frames;
    int n, a; /* Number of frames, and Allocated frames */

} GIF;

/**
 * ## Functions
 */

/**
 * ### `extern int gif_verbose;`
 * Controls the verbosity of output to `stdout` and `stderr`.
 * 0 is no output. 1 and 2 results in more verbose output.
 */
extern int gif_verbose;

/**
 * ### `GIF *gif_create(int w, int h);`
 * Creates a new `GIF` structure of the specified dimensions.
 */
GIF *gif_create(int w, int h);

/**
 * ### `void gif_free(GIF *gif);`
 * Deallocates a `GIF` structure.
 */
void gif_free(GIF *gif);

/**
 * ### `void gif_set_palette(GIF *gif, unsigned int *palette, unsigned int palette_size)`
 * Sets a custom `palette` of the specified `palette_size` for the GIF.
 * If a palette is not specified, one is created from the first frame in the GIF when
 * the file is saved.
 *
 * The colours in `palette` should be in the `0xAARRGGBB` format.
 */
void gif_set_palette(GIF *gif, unsigned int *palette, unsigned int palette_size);

/**
 * ### `GIF *gif_load(const char *filename);`
 * Loads the specified GIF file into a new `GIF` structure.
 */
GIF *gif_load(const char *filename);

/**
 * ### `int gif_save(GIF *g, const char *filename);`
 * Saves a `GIF` structure to a GIF file.
 */
int gif_save(GIF *g, const char *filename);

/**
 * ### `GIF_FRAME *gif_add_frame(GIF *g, Bitmap *b);`
 * Adds a new animation frame to the GIF object.
 * The bitmap is copied internally, and scaled to match the `GIF`'s dimensions.
 */
GIF_FRAME *gif_add_frame(GIF *g, Bitmap *b);

/**
 * ### `GIF_FRAME *gif_new_frame(GIF *g);`
 * Creates a new blank animation frame of the GIF's dimensions.
 */
GIF_FRAME *gif_new_frame(GIF *g);
