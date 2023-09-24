use clap::{
    command,
    arg,
};
use std::fs;
//use std::io;

//struct NumCLArgsError;

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
            true
        }
        else { false };
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

    let dir: &str = &(std::env::var("HOME").expect("Home directory should exist") + "/.tsudoku");
    if delete_saved_games {
        //for file in fs::read_dir("$HOME/.tsudoku/") {
        //for file in fs::read_dir(".") {
        //if let Ok(dir) = fs::read_dir(".") {
        /*for file in fs::read_dir(".") {
            println!("{}", file.path());
        }*/
        /*for file in fs::read_dir("~").unwrap() {
            println!("{}", file.unwrap().path().display());
        }*/
        //println!("{}", HOME);
        //let dir: fs::ReadDir = fs::read_dir(dir.clone()).expect(&(dir + " doesn't exist"));
        let dir: fs::ReadDir = match fs::read_dir(dir) {
            Ok(list) => list,
            Err(msg) => {
                eprintln!("{}", msg.to_string());
                std::process::exit(1);
                //return Err(&(msg.to_string()));
            },
        };
        //TODO: Only remove *.csv files
        for file in dir {
            println!("{}", file.as_ref().unwrap().path().display());
            let _ = fs::remove_file(file.unwrap().path());
        }
    }
    //TODO: Create ~/.tsudoku directory, if it doesn't already exist

    Ok(())
}
