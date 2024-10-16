//include!(concat!(env!("OUT_DIR"), "/bindings.rs"));
use clap;
use std::{
    fs,
    path::PathBuf,
};
use menu::{
    Menu,
    MenuOption,
    MainMenu,
    MainMenuOption,
    SavedGameMenu,
    SavedGameMenuOption,
};
use common::DIR;
use sudoku::{
    Sudoku,
    SavedPuzzle,
};
use terminal::display::{
    self,
    CURSOR_VISIBILITY,
};

pub mod menu;
pub mod terminal;
pub mod common;
pub mod sudoku;

/*extern "C" {
    fn clear ();    //ncurses.h
}*/
const save_file_name: &str = "completed-puzzles.txt";

fn main() -> Result<(), &'static str> {
    // TODO: Clean up unused comments
    // TODO: Add in any missing doc comments
    // TODO: Fix formatting
    // TODO: Fix all compiler warnings
    /* TODO: The best way to handle removing all save game data is probably via a new command
     *       line argument. If that argument is detected, then $HOME/.tsudoku gets removed and
     *       the program immediately ends. It should probably ask for double confirmation as
     *       well.
     *       v2.1.0
     */
    let matches = clap::command!()
        .arg(
            clap::arg!(-n --"no-in-game-menu"
                "Disables the in-game menu, allowing a smaller terminal window"
            )
            .required(false)
        )
        .arg(
            clap::arg!(-d --"delete-saved-games" "Deletes all saved game data")
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
    //println!("ncurses::KEY_ENTER: {}", ncurses::KEY_ENTER);
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
                MainMenuOption::NEW_GAME => {
                    let mut puzzle: Sudoku = Sudoku::new(None);
                    if puzzle.start_game(use_in_game_menu, None) {
                        increment_completed_games();
                    }
                },
                MainMenuOption::RESUME_GAME => {
                    let saved_game_menu: SavedGameMenu = SavedGameMenu::new();
                    if let MenuOption::SAVED_GAME_MENU(SavedGameMenuOption::SAVE_READY) = saved_game_menu.menu() {
                        let saved_puzzle: SavedPuzzle = saved_game_menu.get_saved_game();
                        let mut puzzle: Sudoku = Sudoku::new(Some(saved_puzzle));
                        if puzzle.start_game(use_in_game_menu, None) {
                            increment_completed_games();
                            fs::remove_dir_all(DIR().join(puzzle.filename()))
                                .expect("Error: Issue removing save game files");
                        }
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

    display::clear();
    display::tui_end();
    Ok(())
}

/**
 * Reads in the current number of games the player has successfully completed and then displays that
 * information to the screen in the terminal window.
 */
fn display_completed_puzzles () {
    let num_completed: Result<String, _> = fs::read_to_string(DIR().join(save_file_name));
        // .expect("Error 404: File Not Found");
    //TODO: Keep this around for later when I have to update the number read in
    //let num_completed: u64 = num_completed[..num_completed.len() - 1].parse()
    //    .expect("Unable to parse number of completed puzzles");

    let prompt1: String = format!(
        "Completed Sudoku puzzles: {}",
        match num_completed {
            Ok(num) => num,
            Err(_) => String::from("0"),
        }
    );
    let prompt2: &str = "Press Enter to continue";

    /*let mut y_max: i32 = 0;
    let mut x_max: i32 = 0;
    getmaxyx(stdscr(), &mut y_max, &mut x_max);*/
    let (y_max, x_max): (i32, i32) = display::get_max_yx();

    display::curs_set(CURSOR_VISIBILITY::NONE);
    display::clear();
    display::mvprintw(y_max/2, x_max/2 - (prompt1.len() as i32 - 1)/2, &prompt1);
    display::mvprintw(y_max/2 + 2, x_max/2 - prompt2.len() as i32/2, prompt2);
    display::refresh();

    loop {
        match display::getch().unwrap() {
            display::Input::KeyEnter => break,
            _ => (),
        }
    }
    display::curs_set(CURSOR_VISIBILITY::BLOCK);
}

/**
 * Increments the number of completed games recorded by 1. This is only called once the user has
 * solved the current puzzle.
 */
fn increment_completed_games () {
    let path: PathBuf = DIR().join(save_file_name);
    let num_completed: Result<String, _> = fs::read_to_string(path.clone());
    let num_completed: u128 = match num_completed {
        Ok(num) => num.trim_end().parse()
                        .expect("Error: Unable to parse number of completed puzzles"),
        Err(_) => 0,
    };

    fs::write(path, format!("{}\n", num_completed + 1))
        .expect("Error: Unable to update # completed puzzles");
}
