# sudoku-tui

This will be a user-friendly implementation of the classic game of sudoku that you can play in your terminal. As such, it will be all text-based, but should be extremely easy to play. More in-depth explanations as far as how to play the game will be provided, assuming I remember to do so (I'm currently going through graduate school, so it's very likely that I'll forget to update this or will go long stretches of time without any updates).

# Status update: 15 July 2022

The ncurses library has been successfully implemented, mainly in that iostream library calls have been converted to ncurses equivalents. It wasn't as difficult as I expected it to be.

Next up will either be getting regular output (what the player will actually see) working instead of using debug mode or getting interactivity (allowing the player to navigate and input/change values) working. I haven't decided yet.
