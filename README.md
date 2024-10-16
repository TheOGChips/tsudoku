# tsudoku

## Status Update: 12 October 2024

Good news! The Rust port of `tsudoku` is now fully playable and ready for an official v2.0.0 release! This means that the original C++ version will no longer be maintained, and as the porting process revealed it was a bit of a buggy mess anyway. The new Rust port will still have a few updates lined up, and this README will be updated accordingly. Anyone is welcome to notify me of a bug fix by email at: github.spokesman589@passmail.net . If you have played `tsudoku` and enjoyed your experience, please let me know via email as well; I'd love to hear about it! And if you feel obligated to compensate me for my time working on this, you can also email me to ask about setting up a one-time donation. Have fun playing!

## Description

tsudoku (both short for "terminal sudoku" and inspired phonetically by other Japanese words such as "tsunami") is a user-friendly implementation of the classic game of sudoku that you can play in your terminal.

## What happened to v1.x?

If you're wondering what happened to v1.x, that is the original C++ version. Since all that work wasn't tracked in crates.io, it will look like v1.x is missing; however, anyone who wants to see the v1.x source code can still find it on this project's Github page.

## Supported systems

The below operating systems have been tested for working condition. Other Unix-based OSs are also likely to also be supported without much hindrance. The new Rust version should also work in Windows thanks to the use of the `pancurses` library, but that has yet to be tested.

>\- Debian Linux  
>\- WSL (Windows Subsystem for Linux)

**Note for MacOS users**: It's possible that this README will be marked as executable if you clone this repo directly and prevent you from easily viewing the file in a text editor. Alternative ways to view the files marked this way include the Unix command line utility `less` or terminal-based text editors such as `vim`/`view` or `nano`. The easiest and simplest to use is `less`. Examples are below:

>`$ less README.md`  
>`$ view README.md`  
>`$ nano -v README.md`

**Note for inexperienced WSL users**: tsudoku will not be compilable or playable unless a WSL shell is opened first. A WSL shell should be easily accessible from either a PowerShell or Windows Command Prompt by running `wsl.exe` at the current terminal prompt. Also see the example below. If you are unsure how to install or setup WSL, please reference https://learn.microsoft.com/en-us/windows/wsl/install .

- Ex: Running tsudoku from Windows Command Prompt
>`>wsl.exe`  
>`$ tsudoku`

## Prerequisites

NOTE: See **Prerequisites Installation** section for install instructions.

>\- The Rust compiler: `rustc`

>\- The Rust package manager: `cargo`

>\- A C compiler: `gcc` via `build-essential`

>\- A C-based TUI library: `libncurses-dev`

>\- `curl`

## Prerequisites Installation

>\- For `rustc` and `cargo`, follow the instructions at the official Rust website here: https://www.rust-lang.org/tools/install . The standard installation will work just fine for installing `tsudoku`. Follow the instructions after installing for sourcing `cargo` in the current terminal shell (closing out of the terminal and starting a new instance should also work).

>\- `sudo apt install build-essential libncurses-dev curl`

## Installation

### From crates.io (recommended)

`$ cargo install tsudoku`

### From source

#### Step 1: Clone the project from Github.

- Option 1: Git via HTTPS
>`$ git clone https://github.com/TheOGChips/tsudoku.git && cd tsudoku`

- Option 2: Git via SSH
>`$ git clone git@github.com:TheOGChips/tsudoku.git && cd tsudoku`

- Option 3: Direct download from the main branch
>`$ wget https://github.com/TheOGChips/tsudoku/archive/refs/heads/main.zip -O tsudoku.zip`  
>`$ unzip tsudoku.zip && mv tsudoku-main tsudoku && cd tsudoku`

#### Step 2: Compile the source code using `cargo`. Performing any of the three methods above should have already placed you inside the *tsudoku* directory of the cloned source code.

>`$ cargo build --release`

#### Step 3: Create a symbolic link to the *tsudoku* executable

>`$ mkdir -p $HOME/.local/bin`
>`$ ln -s target/release/tsudoku $HOME/.local/bin`

#### Step 4: Source your shell's RC file to allow running *tsudoku* in the current shell instance.

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
>`$ cargo clean && cargo build --release`

#### If installed via direct downloaded (option 3), delete the *tsudoku* directory and repeat the installation process as described in the **Installation** section again.

>`$ rm -rf tsudoku`  
>`$ wget https://github.com/TheOGChips/tsudoku/archive/refs/heads/main.zip -O tsudoku.zip`  
>`$ unzip tsudoku.zip && mv tsudoku-main tsudoku && cd tsudoku`  
>`$ cargo build --release`

