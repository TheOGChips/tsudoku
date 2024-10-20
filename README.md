# tsudoku

## Description

`tsudoku` (both short for "terminal sudoku" and inspired phonetically by other Japanese words such as "tsunami") is a user-friendly implementation of the classic game of sudoku that you can play in your terminal.

## Supported/Tested systems

The below operating systems have been tested for working condition. Other Unix-based OSs are also likely to also be supported without much hindrance. The new Rust version should also work in Windows thanks to the use of the `pancurses` library, although you will likely need a Curses library compatible with Windows (e.g. `pdcurses`).

>\- Debian Linux  
>\- WSL (Windows Subsystem for Linux)

**Note for MacOS users**: It's possible that this README will be marked as executable if you clone this repo directly and prevent you from easily viewing the file in a text editor. Alternative ways to view the files marked this way include the Unix command line utility `less` or terminal-based text editors such as `vim`/`view` or `nano`. The easiest and simplest to use is `less`. Examples are below:

>`$ less README.md`  
>`$ view README.md`  
>`$ nano -v README.md`

**Note for inexperienced WSL users**: tsudoku will not be compilable or playable unless a WSL shell is opened first. A WSL shell should be easily accessible from either a PowerShell or Windows Command Prompt by running `wsl.exe` at the current terminal prompt. Also see the example below. If you are unsure how to install or setup WSL, please reference [Microsoft's instructions](https://learn.microsoft.com/en-us/windows/wsl/install). It's recommended to install on Debian Bullseye or newer.

- Ex: Running tsudoku from Windows Command Prompt or PowerShell.

>`> wsl.exe '~/.cargo/bin/tsudoku'`

This can also be found in `tsudoku.bat`, which is also provided for Windows/WSL users. Double-clicking on this batch file should open up a Windows Terminal and automatically start tsudoku. Note that quitting tsudoku will also automatically close the instance/tab in Windows Terminal.

**Note for PowerShell users**: An alternative method to get a shortcut to `tsudoku` is to add the following function to your PowerShell profile. This file can be found by running `> $env:USERPROFILE` at a PowerShell prompt. This will allow you to run tsudoku directly from PowerShell in one step. Note that you will have to start a new instance of PowerShell after adding this to your PowerShell profile.

>`function tsudoku { wsl.exe '~/.cargo/bin/tsudoku' }`

## Prerequisites

>\- The Rust compiler: `rustc`

>\- The Rust package manager: `cargo`

>\- A C compiler: `gcc` via `build-essential`

>\- A C-based TUI library: `libncurses-dev`

>\- `curl`

## Installation

### From GitHub (recommended)

Because `tsudoku` requires a few external dependencies, the recommended (and most streamlined) way to install `tsudoku` is via the shell script `tsudoku.sh`. This script will automatically install the required dependencies as well as `tsudoku`. This shell script can also be used to upgrade and uninstall `tsudoku`. For those that wish to be more involved in the install process, continue on to the **From Crates.io** subsection.

#### Step 1: Clone the project from Github.

- Option 1: Git via HTTPS
>`$ git clone https://github.com/TheOGChips/tsudoku.git && cd tsudoku`

- Option 2: Git via SSH
>`$ git clone git@github.com:TheOGChips/tsudoku.git && cd tsudoku`

- Option 3: Direct download from the main branch
>`$ wget https://github.com/TheOGChips/tsudoku/archive/refs/heads/main.zip -O tsudoku.zip`  
>`$ unzip tsudoku.zip && mv tsudoku-main tsudoku && cd tsudoku`

#### Step 2: Compile the source code using the provided shell script. Performing any of the three methods above should have already placed you inside the *tsudoku* directory of the cloned repository.

>`$ ./tsudoku.sh install`

### From [Crates.io](https://crates.io/crates/tsudoku)

See the README at the [`rust-port`](https://github.com/TheOGChips/tsudoku/tree/rust-port) branch.

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

### From GitHub

#### If installed using Git via HTTPS or SSH (options 1 or 2), navigate to the *tsudoku* directory, then perform the following steps:

>`$ git pull`  
>`$ tsudoku.sh upgrade`

#### If installed via direct download (option 3), delete the *tsudoku* directory and repeat the installation process as described in the **Installation** section again.

>`$ rm -rf tsudoku`  
>`$ wget https://github.com/TheOGChips/tsudoku/archive/refs/heads/main.zip -O tsudoku.zip`  
>`$ unzip tsudoku.zip && mv tsudoku-main tsudoku && cd tsudoku`  
>`$ tsudoku.sh install`

### From [Crates.io](https://crates.io/crates/tsudoku)

See the README at the [`rust-port`](https://github.com/TheOGChips/tsudoku/tree/rust-port) branch.

## Uninstallation

### From GitHub

#### If installed using Git via HTTPS or SSH (options 1 or 2), navigate to the *tsudoku* directory, then perform the following steps:

>`$ tsudoku.sh remove`

#### If installed via direct download (option 3), delete the *tsudoku* directory after performing the same step for options 1 and 2 above.

### From [Crates.io](https://crates.io/crates/tsudoku)

See the README at the [`rust-port`](https://github.com/TheOGChips/tsudoku/tree/rust-port) branch.

## Status Update: 20 October 2024

### Current release version: 2.2.3

Good news! The Rust port of `tsudoku` is now complete, fully playable, and ready for an official v2.0.0 release! This means that the original C++ version will no longer be maintained, and as the porting process revealed it was a bit of a buggy mess anyway. Anyone is welcome to notify me of a bug fix by [email](github.spokesman589@passmail.net). If you have played `tsudoku` and enjoyed your experience, please let me know via email as well; I'd love to hear about it! Have fun playing!

### What happened to v1.x?

If you're wondering what happened to v1.x, that is the original C++ version. Since all that work wasn't tracked in [Crates.io](https://crates.io/crates/tsudoku), it will look like v1.x is missing; however, anyone who wants to see the v1.x source code can still find it on this project's Github page.

