#ifndef PUZZLE_H
#define PUZZLE_H

#include <stddef.h>
#include <stdint.h>

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

#define REPORT_ERROR do { fprintf(stderr, "ERROR: %d | %s\n", __LINE__, __FILE__);} while (0)

typedef enum {
    ADD,
    SUB,
} Mode;

typedef unsigned int uint;

void grid_reset(void);
void grid_undo(void);
void place_tile(void);
void cur_tile_change(const Mode m);
void grid_destroy(void);
int grid_create(const uint degree);
void grid_draw(void);

#endif
