# tsudoku

tsudoku is a user-friendly implementation of the classic game of sudoku that you can play in your terminal. (WARNING: I'm currently going through graduate school, so it's very likely that I'll forget to update this or will go long stretches of time without any updates).

Requirements: g++ (tsudoku compiles using the C++17 standard)
              ncurses
              NOTE: These are likely already installed if you're running Linux or MacOS. If not, you should be able to download them easily. There is currently no support for Windows.
              
To run (from the directory tsudoku is in):
    Normal mode:     $ ./tsudoku
    No in-game menu: $ ./tsudoku -n
                     $ ./tsudoku --no-in-game-menu
    Display help:    $ ./tsudoku --help
                     $ ./tsudoku -?

# Status update: 6 February 2023

Great news: tsudoku is now ready for play testing! Anyone who is playing tsudoku after this update is free to play and submit any feedback to myself as desired. I hope everyone likes it.

I still have some stuff that I need/would like to do, but most of that will be transparent to the player (unless it completely breaks the game, in which case it will be opaque). Unless there are any game-breaking bugs that need to be fixed, the next update will follow once these remaining things are taken care of and tsudoku is ready for its first official release.
