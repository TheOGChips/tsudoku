# tsudoku

## Description

`tsudoku` (both short for "terminal sudoku" and inspired phonetically by other Japanese words such as "tsunami") is a user-friendly implementation of the classic game of sudoku that you can play in your terminal.

## Supported/Tested systems

The below operating systems have been tested for working condition. Other Unix-based OSs are also likely to also be supported without much hindrance. The new Rust version should also work in Windows thanks to the use of the `pancurses` library, although you will likely need a Curses library compatible with Windows (e.g. `pdcurses`).

- Debian Linux
- Rocky Linux
- openSUSE Tumbleweed
- FreeBSD
- WSL (Windows Subsystem for Linux)
- Manjaro

**Note for MacOS users**: It's possible that this README will be marked as executable if you clone this repo directly and prevent you from easily viewing the file in a text editor. Alternative ways to view the files marked this way include the Unix command line utility `less` or terminal-based text editors such as `vim`/`view` or `nano`. The easiest and simplest to use is `less`. Examples are below:

>`$ less README.md`  
>`$ view README.md`  
>`$ nano -v README.md`

**Note for inexperienced WSL users**: tsudoku will not be compilable or playable unless a WSL shell is opened first. A WSL shell should be easily accessible from either a PowerShell or Windows Command Prompt by running `wsl.exe` at the current terminal prompt. Also see the example below. If you are unsure how to install or setup WSL, please reference [Microsoft's instructions](https://learn.microsoft.com/en-us/windows/wsl/install). It's recommended to install on Debian Bullseye or newer.

- Ex: Running tsudoku from Windows Command Prompt or PowerShell.

>`> wsl.exe ~/.cargo/bin/tsudoku`

This can also be found in `tsudoku.bat` on the [`main`](https://github.com/TheOGChips/tsudoku) branch, which is also provided for Windows/WSL users. Double-clicking on this batch file should open up a Windows Terminal and automatically start tsudoku. Note that quitting tsudoku will also automatically close the instance/tab in Windows Terminal.

**Note for PowerShell users**: An alternative method to get a shortcut to `tsudoku` that will also accept command line flags is to add the following function to your PowerShell profile. This file can be found by running `> $env:USERPROFILE` at a PowerShell prompt. This will allow you to run tsudoku directly from PowerShell in one step. Note that you will have to start a new instance of PowerShell after adding this to your PowerShell profile.

>`function tsudoku { wsl.exe ~/.cargo/bin/tsudoku $args }`

## Prerequisites

- The Rust compiler: `rustc`
- The Rust package manager: `cargo`
- A C compiler: `gcc` or `clang`
- A C-based TUI library: `ncurses`
- `curl`

## Installation

### From GitHub (recommended)

See the README at the [`main`](https://github.com/TheOGChips/tsudoku) branch.

### From [Crates.io](https://crates.io/crates/tsudoku)

#### Prerequisites Installation

NOTE: The following example assumes Debian or a Debian-based OS using the APT package manager.

>`$ sudo apt install build-essential libncurses-dev curl`

>\- For `rustc` and `cargo`, follow the [instructions](https://www.rust-lang.org/tools/install) at the official Rust website. The standard installation will work just fine for installing `tsudoku`. Follow the instructions after installing for sourcing `cargo` in the current terminal shell (closing out of the terminal and starting a new instance should also work).

NOTE: Source your shell's RC file to allow running *tsudoku* in the current shell instance.

- Examples:
>`$ source ~/.bashrc    # if using bash`  
>`$ source ~/.zshrc     # if using zsh`

#### Install via Cargo

`$ cargo install tsudoku`

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

See the README at the [`main`](https://github.com/TheOGChips/tsudoku) branch.

### From Crates.io

#### Run the following sequence of commands to update `tsudoku` manually

>`$ cargo uninstall tsudoku && cargo install tsudoku`

## Uninstallation

### From GitHub

See the README at the [`main`](https://github.com/TheOGChips/tsudoku) branch.

### From Crates.io

#### Run the following sequence of commands to completely uninstall `tsudoku`

>`$ rm -rf ~/.tsudoku`
>`$ cargo uninstall tsudoku`

## Status Update: 21 November 2024

### Current release version: 2.2.10

Good news! The Rust port of `tsudoku` is now complete, fully playable, and ready for an official v2.0.0 release! This means that the original C++ version will no longer be maintained, and as the porting process revealed it was a bit of a buggy mess anyway. Anyone is welcome to notify me of a bug fix by [email](github.spokesman589@passmail.net). If you have played `tsudoku` and enjoyed your experience, please let me know via email as well; I'd love to hear about it! Have fun playing!

### What happened to v1.x?

If you're wondering what happened to v1.x, that is the original C++ version. Since all that work wasn't tracked in [Crates.io](https://crates.io/crates/tsudoku), it will look like v1.x is missing; however, anyone who wants to see the v1.x source code can still find it on this project's Github page.

