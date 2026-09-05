/*  Compile with:
gcc -Wall -Wextra -Wpedantic -Wconversion -Werror *.c -o main $(pkg-config --cflags --libs raylib)
*/

#include "raylib.h"
#include "puzzle.h"

int main(void) {
    InitWindow(WINDOW_WIDTH,WINDOW_HEIGHT, "Partridge Puzzle");
    SetTargetFPS(60);
    ToggleFullscreen();

    if (grid_create(9)) goto cleanup;

    while (!WindowShouldClose())
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            place_tile();
        }
        if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_DOWN)) {
            cur_tile_change(SUB);
        } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_UP)) {
            cur_tile_change(ADD);
        }
        if (IsKeyPressed(KEY_BACKSPACE))
            grid_reset();
        if (IsKeyPressed(KEY_EIGHT)) {
            grid_destroy();
            grid_create(8);
        } else if (IsKeyPressed(KEY_NINE)) {
            grid_destroy();
            grid_create(9);
        } else if (IsKeyPressed(KEY_ZERO)) {
            grid_destroy();
            grid_create(10);
        }
        if (IsKeyPressed(KEY_U))
            grid_undo();

        BeginDrawing();
        ClearBackground((Color){50, 50, 50, 255});
        grid_draw();

        EndDrawing();
    }

cleanup:
    grid_destroy();
    CloseWindow();
    return 0;
}
