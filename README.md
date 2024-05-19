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

**Note for inexperienced WSL users**: tsudoku will not be compilable or playable unless a WSL shell is opened first. A WSL shell should be easily accessible from either a PowerShell or Windows Command Prompt by running `wsl.exe` at the current terminal prompt. Also see the example below. If you are unsure how to install or setup WSL, please reference https://learn.microsoft.com/en-us/windows/wsl/install . It's recommended to install on Debian Bullseye or newer.

- Ex: Running tsudoku from Windows Command Prompt or PowerShell.

>`> wsl.exe '~/.local/tsudoku'`

**Note for PowerShell users**: It's recommended to add the following function to your PowerShell profile. This file can be found by running `> $env:USERPROFILE` at a PowerShell prompt. This will allow you to run tsudoku directly from PowerShell in one step. Note that you will have to start a new instance of PowerShell after adding this to your PowerShell profile.

>`function { wsl.exe '~/.local/tsudoku' }`

## Prerequisites

NOTE: These are likely already installed depending on your operating system. If not, you should be able to download them easily using your OS's package manager (see **Prerequisites Installation** section).

>\- The GNU C++ compiler `g++` (tsudoku compiles using the C++17 standard)  
>\- NCurses C API
>\- GNU Make
>\- The `unzip` utility

## Prerequisites Installation

If either of the prerquisites are not pre-installed by your operating system, you can install them from your package manager. The following example assumes Debian or a Debian-based OS using the APT package manager.

>`$ sudo apt install g++`  
>`$ sudo apt install libncurses-dev`
>`$ sudo apt install make`
>`$ sudo apt install unzip`

**Note for MacOS Users**: The best option for a MacOS package manager is Homebrew and is relatively similar to a Linux package manager such as that found on Linux and WSL. MacOS should come pre-installed with `clang++`, which should be mapped to `g++`. You can use this instead if you wish. Homebrew's version of an NCurses package is likely just called `ncurses`.

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

#### Step 3: Source your shell's RC file to allow running *tsudoku* in the current shell instance.

- Examples:
>`$ source ~/.bashrc    # if using bash`  
>`$ source ~/.zshrc     # if using zsh`

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

## Status update: 18 May 2024

Current release version: 1.0.6

tsudoku is now ready for official release! Barring any potential future bug fixes (except for a handy couple of additions for you WSL users in the near future), this will be the last update for the C++ version until the Rust port is finished. Anyone is welcome to notify me of a bug fix by email at: github.spokesman589@passmail.net .

If you have played tsudoku and enjoyed your experience, and you feel obligated to compensate me for my time working on this, you can also email me to ask about setting up a one-time donation. Have fun playing!
