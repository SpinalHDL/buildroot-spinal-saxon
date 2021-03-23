#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/time.h>

#define ABS(X) (((X) < 0) ? (-1 * (X)) : (X))
#define MAX_ITER 100

typedef struct {
    double re;
    double im;
} complex_t;

/* Global variables */
volatile sig_atomic_t interrupted = 0;
struct fb_var_screeninfo info;
struct fb_fix_screeninfo finfo;
double domain_min;
double domain_max;
double range_min;
double range_max;
double domain_normal;
double range_normal;

void handler () {
    interrupted = 1;
}

/* Creates an rgb color from hsv values */
unsigned short color (int h, float s, float v) {
    unsigned short ret = 0;
    double c;
    double x;
    double m;
    double rp;
    double gp;
    double bp;
    unsigned char r;
    unsigned char g;
    unsigned char b;

    c = v * s;
    x = c * (1 - ABS(((h / 60) % 2) - 1));
    m = v - c;

    h %= 360;
    if (0 <= h && h < 60) {
        rp = c;
        gp = x;
        bp = 0;
    } else if (60 <= h && h < 120) {
        rp = x;
        gp = c;
        bp = 0;
    } else if (120 <= h && h < 180) {
        rp = 0;
        gp = c;
        bp = x;
    } else if (180 <= h && h < 240) {
        rp = 0;
        gp = x;
        bp = c;
    } else if (240 <= h && h < 300) {
        rp = x;
        gp = 0;
        bp = c;
    } else if (200 <= h && h < 360) {
        rp = c;
        gp = 0;
        bp = x;
    }

    r = (rp + m) * 31;
    g = (gp + m) * 63;
    b = (bp + m) * 31;

    ret |= r << 11;
    ret |= g << 5;
    ret |= b;

    return ret;
}

/* Turns row,col into an absolute position */
unsigned int position (unsigned int row, unsigned int col) {
    return (row * (finfo.line_length / (info.bits_per_pixel / 8))) + col;
}

/* Turns a complex point into an absolute position */
unsigned int c_position (complex_t c) {
    double domain_percent;
    double range_percent;

    /* Return -1 if the position is outside the domain/range */
    if (c.re < domain_min || c.re > domain_max) return -1;
    else if (c.im < range_min || c.im > range_max) return -1;

    /* Fix the point */
    c.re -= domain_min;
    c.im -= range_min;

    /* Calculate how far in the point is compared the domain/range */
    domain_percent = c.re / domain_normal;
    range_percent = 1 - (c.im / range_normal);

    return position ((unsigned int)(range_percent * info.yres_virtual),
                     (unsigned int)(domain_percent * info.xres_virtual));
}

/* Returns the magnitude of a complex number */
double mag (complex_t c) {
    return sqrt ((c.re * c.re) + (c.im * c.im));
}

double mag2 (complex_t c) {
    return  ((c.re * c.re) + (c.im * c.im));
}

/* Multiplies two complex numbers */
complex_t mult (complex_t a, complex_t b) {
    complex_t ret = {
        .re = 0,
        .im = 0
    };

    ret.re = (a.re * b.re) - (a.im * b.im);
    ret.im = (a.im * b.re) + (a.re * b.im);

    return ret;
}

/* Adds two complex numbers */
complex_t add (complex_t a, complex_t b) {
    complex_t ret = {
        .re = 0,
        .im = 0
    };

    ret.re = a.re + b.re;
    ret.im = a.im + b.im;

    return ret;
}

/* Tests how close to the Mandelbrot set a point is */
unsigned int mandelbrot (complex_t c) {
    complex_t z = {
        .re = 0,
        .im = 0
    };
    unsigned int ret = 0;

    while (mag2(z) <= 4 && ret < MAX_ITER) {
        z = mult (z, z);
        z = add (z, c);
        ret++;
    }

    return ret;
}

/* Draws the pixel to the screen */
void paint (unsigned short *buf, unsigned int pos, unsigned int color) {
    //printf("%d %d\n",buf,pos);
    if (pos != (unsigned int)-1) *(buf + pos) = color;
}

int main () {
    int fb_file; 
    unsigned int *fb;
    unsigned int *buf;
    unsigned int *iterationsBuf;
    sigset_t mask;
    struct sigaction usr_action;
    double x_inc;
    double y_inc;
    complex_t pos;
    unsigned int m;

    const char *CSI = "\x1B[";

    /* Attempt to open the framebuffer */
    fb_file = open ("/dev/fb0", O_RDWR);
    if (fb_file == -1) {
        write (2, "Error opening framebuffer.\n", 27);
        return -1;
    }

    /* Attempt to get information about the screen */
    if (ioctl (fb_file, FBIOGET_VSCREENINFO, &info)) {
        write (2, "Error getting screen var info.\n", 31);
        close (fb_file);
        return -1;
    }
    if (ioctl (fb_file, FBIOGET_FSCREENINFO, &finfo)) {
        write (2, "Error getting screen fix info.\n", 31);
        close (fb_file);
        return -1;
    }

    /* Attempt to mmap the framebuffer */
    fb = mmap (0, finfo.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fb_file, 0);
    if ((long)fb == (long)MAP_FAILED) {
        write (2, "Error mapping framebuffer to memory.\n", 37);
        close (fb_file);
        return -1;
    }

    /* Attempt to create a temp screen buffer */
    buf = calloc (finfo.smem_len, 1);
    if (!buf) {
        write (2, "Error creating temp buffer.\n", 28);
        munmap (fb, finfo.smem_len);
        close (fb_file);
        return -1;
    }

    unsigned int width = info.xres_virtual, height = info.yres_virtual;

    iterationsBuf = calloc (width * height, 4);

    /* Set up the SIGINT handler */
    sigfillset (&mask);
    usr_action.sa_handler = handler;
    usr_action.sa_mask = mask;
    usr_action.sa_flags = 0;
    sigaction (SIGINT, &usr_action, NULL);

    /* Set up the domain and range of the graph */
    domain_min = -2;
    domain_max = 1;
    range_min = -1;
    range_max = 1;

    /* Shift the domain/range so that it starts at 0 */
    domain_normal = domain_max - domain_min;
    range_normal = range_max - range_min;

    /* Calculate x and y increments */
    x_inc = domain_normal / info.xres_virtual;
    y_inc = range_normal / info.yres_virtual;

    /* Hide cursor */
    write (1, CSI, 2);
    write (1, "?25l", 4);

    /* Clear the screen (copies the empty buffer) */
    //memcpy (fb, buf, finfo.smem_len);
    write (1, CSI, 2);
    write (1, "2J", 2);

    /* Move cursor to top left corner */
    write (1, CSI, 2);
    write (1, "H", 1);

    memset (fb, 0, finfo.smem_len);

    /* Test every point
     * Draw to a temporary buffer */
    printf("Start draw\n");
    struct timeval t1;
    gettimeofday(&t1, NULL);


    int iterations = 0;
    unsigned int *iterationsBufPtr = iterationsBuf;

    pos.im = range_min + y_inc;
    for (unsigned int y = 0; y < height; y+=1) {
        pos.re = domain_min + x_inc;
        for (unsigned int x = 0; x < width; x+=1) {
            m = mandelbrot (pos);
            *iterationsBufPtr++ = m;
            iterations += m;
            pos.re += x_inc;
        }
        pos.im += y_inc;
    }


    struct timeval t2;
    gettimeofday(&t2, NULL);

    for(unsigned int i = 0;i < width*height;i++){
        unsigned int m = iterationsBuf[i];
        paint ((unsigned short*)fb, i, color (360 * m / MAX_ITER, 1, (m < MAX_ITER) ? 1 : 0));
    }



    double elapsedTime;
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    printf("%d iterations.\n", iterations);
    printf("%d ms.\n", (int)elapsedTime);
    printf("%d ns/iterations.\n", (int)(elapsedTime/iterations*1e6));



//    /* Copy buffer to screen */
//    printf("Start copy\n");
//    memcpy (fb, buf, finfo.smem_len);
//
//    /* Wait for SIGINT */
//    while (!interrupted);
//
//    /* Clear the screen */
//    memset (fb, 0, finfo.smem_len);

    /* Close the framebuffer */
    free (buf);
    munmap (fb, finfo.smem_len);
    close (fb_file);

    /* Show cursor */
    write (1, CSI, 2);
    write (1, "?25h", 4);

    return 0;
}
