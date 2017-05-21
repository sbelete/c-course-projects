#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "./fractal.h"
#include "./image.h"

/*

    TODO: define a struct which wraps the arguments to generate_fractal_region()

*/
    struct indivual {
        color_t *idata;
        float w;
        float h;
        float start;
        unsigned int r;
    };

/*
    generates part of the fractal region, computing data for a number of rows
    beginning from start_y
    
    arguments:    data: a struct wrapping the data needed to generated the fractal
                    - a pointer to the color data region of memory
                    - the width of the image
                    - the height of the image
                    - the starting row for filling out data
                    - the number of rows to fill out
    returns:    NULL
*/
void *gen_fractal_region(void *data) {
    /* TODO: unpack the data struct and call generate_fractal_region()    */
    struct indivual *ptr = data;
    color_t *idata = ptr->idata;
    float w   = ptr->w;
    float h  = ptr->h;
    float start = ptr->start;
    unsigned int r = ptr->r;
    generate_fractal_region(idata, w, h, start, r);
    free(ptr);

    return NULL;
}



int generate_fractal(char *file, float width, float height, int workers) {
    /* TODO: initialize several threads which will compute a unique
             region of the fractal data, and have them each execute
             gen_fractal_region().    */
    color_t *fdata = malloc(sizeof(color_t)*width*height);
    pthread_t thread[workers];
    for(int i=0;i<workers;i++) {
        struct indivual *reg = malloc(sizeof(struct indivual));
        reg->idata = fdata;
        reg->w   = width;
        reg->h  = height;
        reg->r  = height/workers;
        reg->start = i*height/workers;
        pthread_create(&thread[i],0,gen_fractal_region,reg);
    }
    
    for (int i=0;i<workers;i++) {
            pthread_join(thread[i],0);
    }
    
    /*
        save the generated fractal data into the file specified by argv[1]
    */
    if (save_image_data(file, fdata, width, height)) {
    fprintf(stderr, "error saving to image file.\n");
    free(fdata);
    return 1;
    }
    free(fdata);
    
    
    printf("Complete.\n");
    return 0;
}
