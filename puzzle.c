#include "puzzle.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "raylib.h"

#define MAX_TILE_WIDTH 14

static uint** grid = NULL;
static size_t grid_width = 0;
static uint grid_degree = 0;
static uint** undo_grid = NULL;
static uint move_number = 1;

static int unit_length = 0;

static uint cur_tile = 1;
typedef struct Color Color;

Color COLORS[MAX_TILE_WIDTH] = {0};
int tile_inventory[MAX_TILE_WIDTH] = {0};

static inline int get_mouse_unit(const int m)
{
    return m / unit_length;
}

void grid_reset(void)
{
    for (size_t i = 0; i < grid_width; ++i)
        for (size_t j = 0; j < grid_width; ++j)
            grid[i][j] = 0;
    for (int i = 0; i < MAX_TILE_WIDTH; ++i)
        tile_inventory[i] = i + 1;
}

void grid_undo(void)
{
    if (move_number <= 1) return;
    --move_number;
    uint cache_val = 0;
    for (size_t i = 0; i < grid_width; ++i) {
        for (size_t j = 0; j < grid_width; ++j) {
            if (undo_grid[i][j] != move_number) continue;
            cache_val = grid[i][j];
            undo_grid[i][j] = 0;
            grid[i][j] = 0;
        }
    }
    ++tile_inventory[cache_val - 1];
}

void place_tile(void)
{
    const int mx = GetMouseX();
    const int my = GetMouseY();
    if (my < 0 || mx < 0) return;

    if ((get_mouse_unit(mx) + (int)cur_tile - 1) >= (int)grid_width) return;
    if ((get_mouse_unit(my) + (int)cur_tile - 1) >= (int)grid_width) return;

    if (1 > tile_inventory[cur_tile - 1])
        return;

    for (int i = get_mouse_unit(mx); i < get_mouse_unit(mx) + (int)cur_tile; ++i)
        for (int j = get_mouse_unit(my); j < get_mouse_unit(my) + (int)cur_tile; j++)
            if (0 != grid[i][j]) return;
    --tile_inventory[cur_tile - 1];
    for (int i = get_mouse_unit(mx); i < get_mouse_unit(mx) + (int)cur_tile; ++i)
        for (int j = get_mouse_unit(my); j < get_mouse_unit(my) + (int)cur_tile; j++)
            grid[i][j] = cur_tile;
    for (int i = get_mouse_unit(mx); i < get_mouse_unit(mx) + (int)cur_tile; ++i)
        for (int j = get_mouse_unit(my); j < get_mouse_unit(my) + (int)cur_tile; j++)
            undo_grid[i][j] = move_number;
    ++move_number;
}

void cur_tile_change(const Mode m)
{
    if (m == ADD) {
        if ((size_t)cur_tile < grid_degree)
            ++cur_tile;
    } else {
        if (cur_tile > 1)
            --cur_tile;
    }
}

void grid_destroy(void)
{
    if (grid == NULL) goto UNDO;
    for (size_t i = 0; i < grid_width; ++i)
        free(grid[i]);
    free(grid);
    grid = NULL;
UNDO:
    if (undo_grid == NULL) goto END;
    for (size_t i = 0; i < grid_width; ++i)
        free(undo_grid[i]);
    free(undo_grid);
    undo_grid = NULL;
END:
    move_number = 1;
    grid_degree = 0;
    unit_length = 0;
    cur_tile = 1;
    grid_width = 0;
}

int grid_create(const uint degree)
{
    const unsigned int seed = (unsigned int)time(NULL);
    srand(1788555799u);
    printf("Seed: %u\n", seed);

    if (degree > MAX_TILE_WIDTH) return 1;
    grid_width = (degree * (degree + 1)) / 2;
    grid_degree = degree;
    cur_tile = 1;

    grid = calloc(grid_width, sizeof(*grid));
    if (grid == NULL) return 1;
    for (size_t i = 0; i < grid_width; ++i) {
        grid[i] = calloc(grid_width, sizeof(**grid));
        if (grid[i] == NULL) {
            grid_destroy();
            return 1;
        }
    }

    undo_grid = calloc(grid_width, sizeof(*undo_grid));
    if (undo_grid == NULL) return 1;
    for (size_t i = 0; i < grid_width; ++i) {
        undo_grid[i] = calloc(grid_width, sizeof(**undo_grid));
        if (undo_grid[i] == NULL) {
            grid_destroy();
            return 1;
        }
    }

    unit_length = (int)(WINDOW_HEIGHT / grid_width);

    for (int i = 0; i < MAX_TILE_WIDTH; ++i) {
        COLORS[i] = (Color) {
            (unsigned char)(rand() % 255),
            (unsigned char)(rand() % 255),
            (unsigned char)(rand() % 255),
            255
        };

        tile_inventory[i] = i + 1;
    }

    return 0;
}

static inline Color get_cell_color(const int i, const int j, const int mx, const int my)
{
    if (mx >= ((i - (int)cur_tile + 1) * unit_length) && mx < ((i + 1) * unit_length) && my >= ((j - (int)cur_tile + 1) * unit_length) && my < ((j + 1) * unit_length))
        return (Color){140, 140, 140, 255};
    else
        return (Color){50, 50, 50, 255};
}

void grid_draw(void)
{
    const int mx = GetMouseX();
    const int my = GetMouseY();

    for (size_t i = 0; i < grid_width; ++i) {
        for (size_t j = 0; j < grid_width; ++j) {
            Color col;
            if (grid[i][j] == 0) {
                col = get_cell_color((int)i, (int)j, mx, my);
            } else {
                col = COLORS[grid[i][j]];
            }
            DrawRectangle((int)i * unit_length, (int)j * unit_length, unit_length, unit_length, col);
        }
    }
    for (size_t i = 0; i < grid_width + 1; ++i) {
        DrawLine(0, (int)i * unit_length, unit_length * (int)grid_width, (int)i * unit_length, BLACK);
        DrawLine((int)i * unit_length, 0, (int)i * unit_length, unit_length * (int)grid_width, BLACK);
    }

    int y_offset = 0;
    for (int i = 1; (uint)i <= grid_degree; ++i) {
        DrawRectangle((int)grid_width * unit_length + 30, y_offset, unit_length * i, unit_length * i, COLORS[i]);
        DrawText(TextFormat("%d", tile_inventory[i - 1]), (int)grid_width * unit_length + 6, y_offset, 25, RAYWHITE);
        y_offset += unit_length * i;
    }

    DrawRectangle(WINDOW_WIDTH - unit_length * (int)cur_tile, 0, unit_length * (int)cur_tile, unit_length * (int)cur_tile, COLORS[cur_tile]);
}

