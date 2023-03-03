# tsudoku

## Description

tsudoku (both short for "terminal sudoku" and inspired phonetically by other Japanese words such as "tsunami") is a user-friendly implementation of the classic game of sudoku that you can play in your terminal.

## Supported systems

The below operating systems have been tested for working condition. Other Unix-based OSs are also likely to also be supported.

>\- Debian Linux  
>\- MacOS  
>\- WSL (Windows Subsystem for Linux)

**Note for MacOS users**: It's possible that certain (e.g. the Makefile and this README) will be marked as executable during the install process and prevent you from easily viewing the file in a text editor. Alternative ways to view the files marked this way include the Unix command line utility `less` or terminal-based text editors such as `vim`/`view` or `nano`. The easiest and simplest to use is `less`. Examples are below:

>`$ less README.md`  
>`$ view README.md`  
>`$ nano -v README.md`

**Note for inexperienced WSL users**: tsudoku will not be compilable or playable unless a WSL shell is opened first. A WSL shell should be easily accessible from either a PowerShell or Windows Command Prompt by running `wsl.exe` at the current terminal prompt. Also see the example below. If you are unsure how to install or setup WSL, please reference https://learn.microsoft.com/en-us/windows/wsl/install .

- Ex: Running tsudoku from Windows Command Prompt
>`>wsl.exe`  
>`$ tsudoku`

## Prerequisites

NOTE: These are likely already installed depending on your operating system. If not, you should be able to download them easily using your OS's package manager (see **Prerequisites Installation** section).

>\- g++ (tsudoku compiles using the C++17 standard)  
>\- ncurses

## Prerequisites Installation

If either of the prerquisites are not pre-installed by your operating system, you can install them from your package manager.

>`$ sudo apt install g++`  
>`$ sudo apt install libncurses-dev`

## Installation

### From source

#### Step 1: Clone the project from Github.

- Option 1: Git via HTTPS
>`$ git clone https://github.com/TheOGChips/tsudoku.git && cd tsudoku`

- Option 2: Git via SSH
>`$ git clone git@github.com:TheOGChips/tsudoku.git && cd tsudoku`

- Option 3: Direct download from the main branch
>`$ wget https://github.com/TheOGChips/tsudoku/archive/refs/heads/main.zip -O tsudoku.zip`  
>`$ unzip tsudoku.zip && mv tsudoku-main tsudoku && cd tsudoku`

#### Step 2: Compile the source code using the provided makefile. Performing any of the three methods above should have already placed you inside the *tsudoku* directory of the cloned source code.

>`$ make`

## Usage

### With in-game menu enabled:

>`$ tsudoku`

### With in-game menu disabled:

>`$ tsudoku -n`  
>or  
>`$ tsudoku --no-in-game-menu`
                 
### Display usage information:

>`$ tsudoku --help`
    
### Remove all saved games

>`$ tsudoku -d`  
>or  
>`$ tsudoku --delete-saved-games`

## Updates

### From source

#### If installed using Git via HTTPS or SSH (options 1 or 2), navigate to the *tsudoku* directory, then perform the following steps:

>`$ git pull`  
>`$ make upgrade`

#### If installed via direct downloaded (option 3), delete the *tsudoku* directory and repeat the installation process as described in the **Installation** section again.

>`$ rm -rf tsudoku`  
>`$ wget https://github.com/TheOGChips/tsudoku/archive/refs/heads/main.zip -O tsudoku.zip`  
>`$ unzip tsudoku.zip && mv tsudoku-main tsudoku && cd tsudoku`  
>`$ make`

## Status update: 6 February 2023

Great news: tsudoku is now ready for play testing! Anyone who is playing tsudoku after this update is free to play and submit any feedback to myself as desired. I hope everyone likes it.

I still have some stuff that I need/would like to do, but most of that will be transparent to the player (unless it completely breaks the game, in which case it will be opaque). Unless there are any game-breaking bugs that need to be fixed, the next update will follow once these remaining things are taken care of and tsudoku is ready for its first official release.
