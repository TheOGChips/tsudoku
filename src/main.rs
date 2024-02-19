#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
//include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

use clap::{
    command,
    arg,
};
use std::{
    fs,
    //ptr::null,
};
use ncurses::clear;
use menu::{
    Menu,
    MainMenu,
    MainMenuOption,
};

pub mod menu;
pub mod terminal;

/*extern "C" {
    fn clear ();    //ncurses.h
}*/

fn main() -> Result<(), &'static str> {
    let matches = command!()
        .arg(
            arg!(-n --"no-in-game-menu"
                 "Disables the in-game menu, allowing a smaller terminal window"
            )
            .required(false)
        )
        .arg(
            arg!(-d --"delete-saved-games" "Deletes all saved game data")
            .required(false)
        )
        .get_matches();

    let mut num_clargs: u8 = 0; //NOTE: I'm surprised there isn't a builtin way to get this
    let use_in_game_menu: bool =
        if matches.get_flag("no-in-game-menu") {
            num_clargs += 1;
            false
        }
        else { true };
    let delete_saved_games: bool =
        if matches.get_flag("delete-saved-games") {
            num_clargs += 1;
            true
        }
        else { false };

    if num_clargs > 1 {
        return Err("\nError: Too many arguments. Only one argument can be accepted.\n");
    }
    println!("use_in_game_menu: {}", use_in_game_menu);
    println!("delete_saved_games: {}", delete_saved_games);
    println!("ncurses::KEY_ENTER: {}", ncurses::KEY_ENTER);
    //println!("KEY_ENTER: {}", KEY_ENTER);

    let dir: &str = &(std::env::var("HOME").expect("Home directory should exist") + "/.tsudoku");
    if delete_saved_games {
        let dir = match fs::read_dir(dir) {
            Ok(list) => list.filter(
                |file| file.as_ref().unwrap().path().display().to_string().contains(".csv")
            ),
            Err(msg) => {
                eprintln!("{}", msg.to_string());
                std::process::exit(1);
            },
        };
        for file in dir {
            //println!("{}", file.as_ref().unwrap().path().display());
            let _ = fs::remove_file(file.unwrap().path());
        }
    }

    let _ = fs::create_dir(dir);

    let main_menu = MainMenu::new(use_in_game_menu);
    let mut opt: MainMenuOption = MainMenuOption::NEW_GAME;
    while opt != MainMenuOption::EXIT {
        opt = main_menu.menu();
    }
    /*unsafe {
        let mut main_menu = MainMenu::new();
        //Example of how enums and enum classes are handled (look at constified_enums):
        //  https://mdaverde.com/posts/rust-bindgen-enum/
        let mut opt: i32;// = options_NEW_GAME;
        loop {
            opt = main_menu.menu(use_in_game_menu);
            if opt == options_NEW_GAME {
                let mut puzzle = Sudoku::new(null());
                puzzle.start_game(use_in_game_menu, null());
            }
            else if opt == options_RESUME_GAME {
                let mut saved_game_menu = SavedGameMenu::new();
                if saved_game_menu.run_menu() == options_SAVE_READY {
                    let saved_puzzle = saved_game_menu.get_saved_game();
                    let mut puzzle = Sudoku::new(&saved_puzzle);
                    puzzle.start_game(use_in_game_menu, &saved_puzzle);
                }
            }
            else if opt == options_SHOW_STATS {
                display_completed_puzzles();
            }
            else if opt == options_EXIT {
                break;
            }
        }
        clear();
    }*/

    clear();
    Ok(())
}
