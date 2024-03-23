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
    path::PathBuf,
    ptr::null,
};
use ncurses::{
    clear,
    stdscr,
    getmaxyx,
    curs_set, CURSOR_VISIBILITY,
    mvprintw,
    refresh,
    getch,
};
use menu::{
    Menu,
    MenuOption,
    MainMenu,
    MainMenuOption,
    SavedPuzzle,
    SavedGameMenu,
    SavedGameMenuOption,
};
use common::DIR;

pub mod menu;
pub mod terminal;
pub mod common;

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

    if delete_saved_games {
        // Deletes all saved games from the tsudoku environment directory at ~/.tsudoku.
        //TODO: Change "dir" to "saved_games"
        let dir = match fs::read_dir(DIR()) {
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

    /* Creates the tsudoku environment directory in the user's home directory at ~/.tsudoku if it
     * doesn't already exist.
     */
    let _ = fs::create_dir(DIR());

    let main_menu = MainMenu::new(use_in_game_menu);
    loop {
        if let MenuOption::MAIN_MENU(main_menu_option) = main_menu.menu() {
            match main_menu_option {
                //TODO: Convert NEW_GAME & RESUME_GAME
                MainMenuOption::NEW_GAME => (),
                MainMenuOption::RESUME_GAME => {
                    let saved_game_menu: SavedGameMenu = SavedGameMenu::new();
                    if let MenuOption::SAVED_GAME_MENU(SavedGameMenuOption::SAVE_READY) = saved_game_menu.menu() {
                        let saved_puzzle: SavedPuzzle = saved_game_menu.get_saved_game();
                        //TODO: Finish this block after converting over NEW_GAME
                    }
                },
                MainMenuOption::SHOW_STATS => display_completed_puzzles(),
                MainMenuOption::EXIT => break,
            }
        }
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

/**
 * Reads in the current number of games the player has successfully completed and then displays that
 * information to the screen in the terminal window.
 */
fn display_completed_puzzles () {
    let num_completed = fs::read_to_string(DIR().join("completed_puzzles.txt"))
        .expect("Error 404: File Not Found");
    //TODO: Keep this around for later when I have to update the number read in
    //let num_completed: u64 = num_completed[..num_completed.len() - 1].parse()
    //    .expect("Unable to parse number of completed puzzles");

    let prompt1: String = format!("Completed Sudoku puzzles: {}", num_completed);
    let prompt2: &str = "Press Enter to continue";

    let mut y_max: i32 = 0;
    let mut x_max: i32 = 0;
    getmaxyx(stdscr(), &mut y_max, &mut x_max);

    curs_set(CURSOR_VISIBILITY::CURSOR_INVISIBLE);
    clear();
    mvprintw(y_max/2, x_max/2 - (prompt1.len() as i32 - 1)/2, prompt1.as_str());
    mvprintw(y_max/2 + 2, x_max/2 - prompt2.len() as i32/2, prompt2);
    refresh();

    while getch() != terminal::KEY_ENTER {}
    curs_set(CURSOR_VISIBILITY::CURSOR_VISIBLE);
}
