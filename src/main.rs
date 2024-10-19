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
    DifficultyMenuOption,
};
use common::game_dir;
use sudoku::{
    Sudoku,
    SavedPuzzle,
};
use terminal::display::{
    self,
    CursorVisibility,
};

pub mod menu;
pub mod terminal;
pub mod common;
pub mod sudoku;

const SAVE_FILENAME_TOTAL: &str = "completed-puzzles.txt";
const SAVE_FILENAME_EASY: &str = "completed-easy.txt";
const SAVE_FILENAME_MEDIUM: &str = "completed-medium.txt";
const SAVE_FILENAME_HARD: &str = "completed-hard.txt";
const SAVE_FILENAME_EXPERT: &str = "completed-expert.txt";

fn main() -> Result<(), &'static str> {
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
    
    if delete_saved_games {
        // Deletes all saved games from the tsudoku environment directory at ~/.tsudoku.
        let saved_games = match fs::read_dir(game_dir()) {
            Ok(list) => list.filter(
                |file| file.as_ref().unwrap().path().display().to_string().contains(".csv")
            ),
            Err(msg) => {
                eprintln!("{}", msg.to_string());
                std::process::exit(1);
            },
        };
        for dir in saved_games {
            let _ = fs::remove_file(dir.unwrap().path());
        }
    }

    /* Creates the tsudoku environment directory in the user's home directory at ~/.tsudoku if
     * it doesn't already exist.
     */
    let _ = fs::create_dir(game_dir());

    let main_menu = MainMenu::new(use_in_game_menu);
    loop {
        if let MenuOption::MainMenu(main_menu_option) = main_menu.menu() {
            match main_menu_option {
                MainMenuOption::NewGame => {
                    let mut puzzle: Sudoku = Sudoku::new(None);
                    if puzzle.start_game(use_in_game_menu) {
                        increment_completed_games(puzzle.difficulty());
                    }
                },
                MainMenuOption::ResumeGame => {
                    let saved_game_menu: SavedGameMenu = SavedGameMenu::new();
                    if let MenuOption::SavedGameMenu(SavedGameMenuOption::SaveReady) =
                        saved_game_menu.menu() {
                            let saved_puzzle: SavedPuzzle = saved_game_menu.get_saved_game();
                            let mut puzzle: Sudoku = Sudoku::new(Some(saved_puzzle));
                            if puzzle.start_game(use_in_game_menu) {
                                increment_completed_games(puzzle.difficulty());
                                fs::remove_dir_all(game_dir().join(puzzle.filename()))
                                    .expect("Error: Issue removing save game files");
                            }
                        }
                },
                MainMenuOption::ShowStats => display_completed_puzzles(),
                MainMenuOption::Exit => break,
            }
        }
    }
    
    display::clear();
    display::tui_end();
    Ok(())
}

/**
 * Reads in the current number of games the player has successfully completed and then displays
 * that information to the screen in the terminal window.
 */
fn display_completed_puzzles () {
    let title: &str = "Completed Sudoku puzzles:";
    let display_strs: [String; 5] = [
        format!("TOTAL:  {}", match fs::read_to_string(game_dir().join(SAVE_FILENAME_TOTAL)) {
            Ok(num) => num,
            Err(_) => String::from("0"),
        }),
        format!("EASY:   {}", match fs::read_to_string(game_dir().join(SAVE_FILENAME_EASY)) {
            Ok(num) => num,
            Err(_) => String::from("0"),
        }),
        format!("MEDIUM: {}", match fs::read_to_string(game_dir().join(SAVE_FILENAME_MEDIUM)) {
            Ok(num) => num,
            Err(_) => String::from("0"),
        }),
        format!("HARD:   {}", match fs::read_to_string(game_dir().join(SAVE_FILENAME_HARD)) {
            Ok(num) => num,
            Err(_) => String::from("0"),
        }),
        format!("EXPERT: {}", match fs::read_to_string(game_dir().join(SAVE_FILENAME_EXPERT)) {
            Ok(num) => num,
            Err(_) => String::from("0"),
        }),
    ];
    let prompt: &str = "Press Enter to continue";

    let (y_max, x_max): (i32, i32) = display::get_max_yx();

    let max_length: i32 = *std::array::from_fn::<i32, 5, _>(|i| display_strs[i].len() as i32)
        .iter()
        .reduce(|max, i| max.max(i))
        .unwrap();

    display::curs_set(CursorVisibility::None);
    display::clear();
    display::mvprintw(y_max/2, x_max/2 - (title.len() as i32 - 1)/2, title);
    let mut count: i32 = 0;
    for string in &display_strs {
        display::mvprintw(
            y_max/2 + 2 + count,
            x_max/2 - (max_length as i32 - 1)/2,
            &string
        );
        count += 1;
    }
    display::mvprintw(y_max/2 + 3 + count, x_max/2 - (prompt.len() as i32 - 1)/2, prompt);
    display::refresh();

    loop {
        match display::getch().unwrap() {
            display::Input::KeyEnter => break,
            _ => (),
        }
    }
    display::curs_set(CursorVisibility::Block);
}

/**
 * Increments the number of completed games recorded by 1. This is only called once the user has
 * solved the current puzzle.
 */
fn increment_completed_games (difficulty: DifficultyMenuOption) {
    let paths: [PathBuf; 2] = [
        game_dir().join(SAVE_FILENAME_TOTAL),
        match difficulty {
            DifficultyMenuOption::Easy => game_dir().join(SAVE_FILENAME_EASY),
            DifficultyMenuOption::Medium => game_dir().join(SAVE_FILENAME_MEDIUM),
            DifficultyMenuOption::Hard => game_dir().join(SAVE_FILENAME_HARD),
            DifficultyMenuOption::Expert => game_dir().join(SAVE_FILENAME_EXPERT),
        },
    ];
    for path in paths {
        let num_completed: Result<String, _> = fs::read_to_string(path.clone());
        let num_completed: u128 = match num_completed {
            Ok(num) => num.trim_end().parse()
                            .expect("Error: Unable to parse number of completed puzzles"),
            Err(_) => 0,
        };
        fs::write(path, format!("{}\n", num_completed + 1))
            .expect("Error: Unable to update # completed puzzles");
    }
}
