# sudoku-tui

This will be a user-friendly implementation of the classic game of sudoku that you can play in your terminal. As such, it will be all text-based, but should be extremely easy to play. More in-depth explanations as far as how to play the game will be provided, assuming I remember to do so (I'm currently going through graduate school, so it's very likely that I'll forget to update this or will go long stretches of time without any updates).

# Status update: 10 July 2022

A puzzle is now successfully initialized. The initial hint positions and the hints themselves are randomly chosen using pseudo-random number generators. The puzzle successfully updates each row, column, and 3x3 submatrix as necessary. The puzzles are printed out 3 different ways to check that rows, columns, and submatrices are synchronized with each other; however, this view is still a debug-only view and not how I intend the final puzzles to be displayed.

Next will be the incorporation of the ncurses (or another curses-type) library to enable interaction by the user. This will likely be the hardest/lengthiest part.