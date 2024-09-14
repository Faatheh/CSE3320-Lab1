/*
    Draw a rotating donut on text console.
    dependency:
        delay
        fb (uart may work? depending on terminal program)
    CREDITS: see end of the file
*/

#include "debug.h"
#include "plat.h"
#include "utils.h"

#define PIXELSIZE 4 /*ARGB, expected by /dev/fb*/
typedef unsigned int PIXEL;
#define NN 640 // canvas dimension. NN by NN

#include "fb.h"
static inline void setpixel(unsigned char *buf, int x, int y, int pit, PIXEL p) {
    assert(x >= 0 && y >= 0);
    *(PIXEL *)(buf + y * pit + x * PIXELSIZE) = p;
}

static void canvas_init(void) {
    fb_fini();
    // acquire(&mboxlock);      //it's a test. so no lock

    the_fb.width = NN;
    the_fb.height = NN;

    the_fb.vwidth = NN;
    the_fb.vheight = NN;

    if (fb_init() != 0)
        BUG();
}

static void screen_clear(void) {
    PIXEL blk = 0x0;
    int x, y;
    int pitch = the_fb.pitch;
    for (y = 0; y < NN; y++)
        for (x = 0; x < NN; x++)
            setpixel(the_fb.fb, x, y, pitch, blk);
}

extern void fb_print(int *x, int *y, char *s); // mbox.c
// x,y are char coordinates, 80 chars per row
static void print_char(char s, int x, int y) {
    char str[2] = {s, '\0'};
    x *= 10;
    y *= 20;

    fb_print(&x, &y, str);
}

/////////////////////////////////////

#define R(mul, shift, x, y)              \
    _ = x;                               \
    x -= mul * y >> shift;               \
    y += mul * _ >> shift;               \
    _ = (3145728 - x * x - y * y) >> 11; \
    x = x * _ >> 10;                     \
    y = y * _ >> 10;

static char b[1760];        // text buffer (W 80 H 22?
static signed char z[1760]; // z buffer

// draw chars on canvas, as from NJU OS
__attribute__((unused)) 
static void donut_char_canvas(void) {
    int sA = 1024, cA = 0, sB = 1024, cB = 0, _;

    canvas_init();
    while (1) {
        memset(b, 32, 1760);  // text buffer
        memset(z, 127, 1760); // z buffer
        int sj = 0, cj = 1024;
        for (int j = 0; j < 90; j++) {
            int si = 0, ci = 1024; // sine and cosine of angle i
            for (int i = 0; i < 324; i++) {
                int R1 = 1, R2 = 2048, K2 = 5120 * 1024;

                int x0 = R1 * cj + R2,
                    x1 = ci * x0 >> 10,
                    x2 = cA * sj >> 10,
                    x3 = si * x0 >> 10,
                    x4 = R1 * x2 - (sA * x3 >> 10),
                    x5 = sA * sj >> 10,
                    x6 = K2 + R1 * 1024 * x5 + cA * x3,
                    x7 = cj * si >> 10,
                    x = 25 + 30 * (cB * x1 - sB * x4) / x6,
                    y = 12 + 15 * (cB * x4 + sB * x1) / x6,
                    N = (((-cA * x7 - cB * ((-sA * x7 >> 10) + x2) - ci * (cj * sB >> 10)) >> 10) - x5) >> 7;

                int o = x + 80 * y; // xzl: 80 chars per row
                signed char zz = (x6 - K2) >> 15;
                if (22 > y && y > 0 && x > 0 && 80 > x && zz < z[o]) {
                    z[o] = zz;
                    // luminance_index is now in the range 0..11 (8*sqrt(2) = 11.3)
                    // now we lookup the character corresponding to the
                    // luminance and plot it in our output:
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
                }
                R(5, 8, ci, si) // rotate i
            }
            R(9, 7, cj, sj) // rotate j
        }
        R(5, 7, cA, sA);
        R(5, 8, cB, sB);

        screen_clear();
        int y = 0, x = 0;
        for (int k = 0; 1761 > k; k++) {
            if (k % 80) {
                if (x < 50)
                    print_char(b[k], x, y); // xzl: clip x at col 50
                x++;
            } else { // xzl: new row (wont print char
                y++;
                x = 1;
            }
        }

        // screen_refresh();
        ms_delay(100);
    }
}

// same as above, but print chars to uart. need a terminal program (putty)
// that can interpret special chars
__attribute__((unused)) 
static void donut_uart(void) {
    int sA = 1024, cA = 0, sB = 1024, cB = 0, _;

    while (1) {
        memset(b, 32, 1760);  // text buffer
        memset(z, 127, 1760); // z buffer
        int sj = 0, cj = 1024;
        for (int j = 0; j < 90; j++) {
            int si = 0, ci = 1024; // sine and cosine of angle i
            for (int i = 0; i < 324; i++) {
                int R1 = 1, R2 = 2048, K2 = 5120 * 1024;

                int x0 = R1 * cj + R2,
                    x1 = ci * x0 >> 10,
                    x2 = cA * sj >> 10,
                    x3 = si * x0 >> 10,
                    x4 = R1 * x2 - (sA * x3 >> 10),
                    x5 = sA * sj >> 10,
                    x6 = K2 + R1 * 1024 * x5 + cA * x3,
                    x7 = cj * si >> 10,
                    x = 25 + 30 * (cB * x1 - sB * x4) / x6,
                    y = 12 + 15 * (cB * x4 + sB * x1) / x6,
                    N = (((-cA * x7 - cB * ((-sA * x7 >> 10) + x2) - ci * (cj * sB >> 10)) >> 10) - x5) >> 7;

                int o = x + 80 * y; // xzl: 80 chars per row
                signed char zz = (x6 - K2) >> 15;
                if (22 > y && y > 0 && x > 0 && 80 > x && zz < z[o]) {
                    z[o] = zz;
                    // luminance_index is now in the range 0..11 (8*sqrt(2) = 11.3)
                    // now we lookup the character corresponding to the
                    // luminance and plot it in our output:
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
                }
                R(5, 8, ci, si) // rotate i
            }
            R(9, 7, cj, sj) // rotate j
        }
        R(5, 7, cA, sA);
        R(5, 8, cB, sB);

        for (int k = 0; 1761 > k; k++)
            putc(0, k % 80 ? b[k] : 10);
        printf("\x1b[23A");
        ms_delay(100);
    }
}

static PIXEL int2rgb (int value); 

// draw dots on canvas, closer to the original js version (see comment at the end)
__attribute__((unused)) 
void donut_dots(void) {
    int sA = 1024, cA = 0, sB = 1024, cB = 0, _;

    canvas_init();
    while (1) {
        memset(b, 32, 1760);  // text buffer
        memset(z, 127, 1760); // z buffer
        int sj = 0, cj = 1024;
        for (int j = 0; j < 90; j++) {
            int si = 0, ci = 1024; // sine and cosine of angle i
            for (int i = 0; i < 324; i++) {
                int R1 = 1, R2 = 2048, K2 = 5120 * 1024;

                int x0 = R1 * cj + R2,
                    x1 = ci * x0 >> 10,
                    x2 = cA * sj >> 10,
                    x3 = si * x0 >> 10,
                    x4 = R1 * x2 - (sA * x3 >> 10),
                    x5 = sA * sj >> 10,
                    x6 = K2 + R1 * 1024 * x5 + cA * x3,
                    x7 = cj * si >> 10,
                    x = 25 + 30 * (cB * x1 - sB * x4) / x6,
                    y = 12 + 15 * (cB * x4 + sB * x1) / x6,
                    // N = (((-cA * x7 - cB * ((-sA * x7 >> 10) + x2) - ci * (cj * sB >> 10)) >> 10) - x5) >> 7;
                    lumince = (((-cA * x7 - cB * ((-sA * x7 >> 10) + x2) - ci * (cj * sB >> 10)) >> 10) - x5); 
                    // xzl: range likely: <0..~1408, scale to 0..255
                    lumince = lumince<0? 0 : lumince/5; 
                    lumince = lumince<255? lumince : 255; 

                int o = x + 80 * y; // xzl: 80 chars per row
                signed char zz = (x6 - K2) >> 15;
                if (22 > y && y > 0 && x > 0 && 80 > x && zz < z[o]) { // xzl: z depth will control visibility
                    z[o] = zz;
                    // luminance_index is now in the range 0..11 (8*sqrt(2) = 11.3)
                    // now we lookup the character corresponding to the
                    // luminance and plot it in our output:
                    // b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
                    b[o] = lumince;                    
                }
                R(5, 8, ci, si) // rotate i
            }
            R(9, 7, cj, sj) // rotate j
        }
        R(5, 7, cA, sA);
        R(5, 8, cB, sB);

        screen_clear();
        int y = 0, x = 0;
        for (int k = 0; 1761 > k; k++) {
            if (k % 80) {
                if (x < 50) {
                    // to display, scale x by K, y by 2K (so we have a round donut)
                    int K=6, xx=x*K, yy=y*K*2;
                    // PIXEL clr = b[k]; // blue only, simple
                    PIXEL clr = int2rgb(b[k]); // to a color spectrum
                    setpixel(the_fb.fb, xx, yy, the_fb.pitch, clr);
                    setpixel(the_fb.fb, xx+1, yy, the_fb.pitch, clr);
                    setpixel(the_fb.fb, xx, yy+1, the_fb.pitch, clr);
                    setpixel(the_fb.fb, xx+1, yy+1, the_fb.pitch, clr);
                }
                x++;
            } else { 
                y++;
                x = 1;
            }
        }
        ms_delay(500);
    }
}

// value: 0..255, PIXEL: argb
static PIXEL int2rgb (int value) {
    int r,g,b;     
    if (value >= 0 && value <= 85) {
        // Red to Yellow (R stays 255, G increases, B stays 0)
        r = 255;
        g = (value * 3);
        b = 0;
    } else if (value > 85 && value <= 170) {
        // Yellow to Cyan (G stays 255, R decreases, B increases)
        r = 255 - ((value - 85) * 3);
        g = 255;
        b = (value - 85) * 3;
    } else if (value > 170 && value <= 255) {
        // Cyan to Blue (G decreases, B stays 255, R stays 0)
        r = 0;
        g = 255 - ((value - 170) * 3);
        b = 255;
    } else {
        // Value out of range
        r=g=b=0;
    }    
    return (r<<16)|(g<<8)|b; 
}

void donut(void) {
    donut_dots();
    // donut_uart();
    // donut_char_canvas();
}

/// project idea: use ktimer to drive the animation. periodically

/**
 * Original author:
 * https://twitter.com/a1k0n
 * https://www.a1k0n.net/2021/01/13/optimizing-donut.html
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-09-15     Andy Sloane  First version
 * 2011-07-20     Andy Sloane  Second version
 * 2021-01-13     Andy Sloane  Third version
 * 2021-03-25     Meco Man     Port to RT-Thread RTOS
 *
 *
 *  js code for both canvas & text version
 *  https://www.a1k0n.net/js/donut.js
 *
 *  ported by FL
 * From the NJU OS project:
 * https://github.com/NJU-ProjectN/am-kernels/blob/master/kernels/demo/src/donut/donut.c
 *
 */
