This is a game that allows you to try and solve partridge packing puzzles.  
Use the arrow keys to change tile size.  Press 8, 9, or 0, to change the grid size.  
Press backspace to reset the board.  Press u to undo previous action.


If your system is the same as mine and you have raylib installed instead of compiled from source, then compile with
```
gcc -O2 *.c -o main $(pkg-config --cflags --libs raylib)
```


This library depends on, [raylib](https://github.com/raysan5/raylib), a graphics library from Ramon Santamaria.
