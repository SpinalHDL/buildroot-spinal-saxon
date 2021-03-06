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

typedef struct {
    complex_t c, z;
    unsigned int iteration;
    unsigned int index;
} state_t;



#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

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
    } else {
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

#define capture(idx)          \
    states[idx].c = pos;       \
    states[idx].iteration = 0; \
    states[idx].z.im = 0;      \
    states[idx].z.re = 0;      \
    states[idx].index = index; \
    index += 1;              \
    x += 1;                  \
    pos.re += inc_x;         \
    if(x == width){          \
        x = 0;               \
        y += 1;              \
        pos.re = start_x; \
        pos.im += inc_y;     \
    }


//paint ((unsigned short*)fb, states[idx].index, color (360 * states[idx].iteration / MAX_ITER, 1, (states[idx].iteration < MAX_ITER) ? 1 : 0));

#define processA(idx) \
            zz_re[idx] = states[idx].z.re*states[idx].z.re;\
            zz_im[idx] = states[idx].z.im*states[idx].z.im;\
            zz_ri[idx] = states[idx].z.re*states[idx].z.im;

#define processB(idx) \
            zz[idx] = zz_re[idx] +  zz_im[idx];

#define processC(idx) \
            states[idx].z.re = zz_re[idx] - zz_im[idx]; \
            states[idx].z.im = zz_ri[idx] + zz_ri[idx];

#define processD(idx) \
            states[idx].z.re += states[idx].c.re; \
            states[idx].z.im += states[idx].c.im;

#define processE(idx) \
            not_done[idx] = (states[idx].iteration++ < MAX_ITER) & (zz[idx] <= four);

#define processF(idx) \
            if(unlikely(!not_done[idx])){ \
                states[idx].iteration -= 1;\
                buf[states[idx].index] = states[idx].iteration;\
                iterations += states[idx].iteration; \
                capture(idx); \
                if(y == height-1){ \
                    goto mandelbrot_done; \
                } \
            }


#define doThat(idx, that) that;
#define barrier
#define UNUSED(x) (void)(x)

#define UC 4
#define foreachInline(that) \
    that(0) \
    that(1) \
    that(2) \
    that(3)

int __attribute__ ((noinline)) mandelbrotLoopOpt(
    double start_x, double start_y,
    double inc_x, double inc_y,
    unsigned int width, unsigned int height,
    unsigned int *buf, unsigned short * fb){
    unsigned int iterations = 0;
    complex_t pos;
    pos.re = start_x;
    pos.im = start_y;
    unsigned int x = 0, y = 0;
    unsigned int index = 0;
    UNUSED(fb);

    state_t states[UC];

    foreachInline(capture)

    double four = 4.0;
    while(1){
        double zz_re[UC], zz_im[UC], zz_ri[UC];
        double zz[UC];
        unsigned int not_done[UC];

        foreachInline(processA); barrier;
        foreachInline(processB); barrier;
        foreachInline(processC); barrier;
        foreachInline(processD); barrier;
        foreachInline(processE); barrier;
        foreachInline(processF); barrier;
    }
    mandelbrot_done:
    return iterations;
}

//    #define foreach(that)
//    for(int idx = 0;idx < UC;idx++){
//        that;
//    }
//    foreach(
//        capture(idx)
//    )
//        foreach(
//            zz_re[idx] = states[idx].z.re*states[idx].z.re;
//            zz_im[idx] = states[idx].z.im*states[idx].z.im;
//            zz_ri[idx] = states[idx].z.re*states[idx].z.im;
//        )
//
//        foreach(zz[idx] = zz_re[idx] +  zz_im[idx])
//        foreach(done[idx] = (states[idx].iteration >= MAX_ITER) | (zz[idx] > four)) //| instead of || is significant
//        foreach(
//            states[idx].z.re = zz_re[idx] - zz_im[idx];
//            states[idx].z.im = zz_ri[idx] + zz_ri[idx];
//        )
//        foreach(
//            states[idx].z.re += states[idx].c.re;
//            states[idx].z.im += states[idx].c.im;
//            states[idx].iteration += 1;
//        )
//
//        foreach(
//            if(unlikely(done[idx])){
//                states[idx].iteration -= 1;
//                buf[states[idx].index] = states[idx].iteration;
//                //paint ((unsigned short*)fb, states[idx].index, color (360 * states[idx].iteration / MAX_ITER, 1, (states[idx].iteration < MAX_ITER) ? 1 : 0));
//                iterations += states[idx].iteration;
//                capture(idx);
//                if(y == height-1){
//                    goto mandelbrot_done;
//                }
//            }
//        )


int __attribute__ ((noinline)) mandelbrotLoopSimple(
    double start_x, double start_y,
    double inc_x, double inc_y,
    unsigned int width, unsigned int height,
    unsigned int *buf, unsigned short * fb){
    unsigned int iterations = 0;
    complex_t pos;
    UNUSED(fb);
    pos.im = start_y;
    for (unsigned int y = 0; y < height; y+=1) {
        pos.re = start_x;
        for (unsigned int x = 0; x < width; x+=1) {
            unsigned int m = mandelbrot (pos);
            *buf++ = m;
            iterations += m;
            pos.re += inc_x;
        }
        pos.im += inc_y;
    }
    return iterations;
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
    struct timeval t1, t2;
    gettimeofday(&t1, NULL);





    int iterations = mandelbrotLoopOpt(domain_min, range_min, x_inc, y_inc, width, height, iterationsBuf, (unsigned short*)fb);
//    int iterations = mandelbrotLoopSimple(domain_min, range_min, x_inc, y_inc, width, height, iterationsBuf, (unsigned short*)fb);

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
    printf("%d MFLOPS.\n", (int)(iterations*9/elapsedTime*1e-3));



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
