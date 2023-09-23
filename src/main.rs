use clap::{
    command,
    arg,
};

fn main() {
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
        eprintln!("\nError: Too many arguments. Only one argument can be accepted.\n");
        return;
    }
    println!("use_in_game_menu: {}", use_in_game_menu);
    println!("delete_saved_games: {}", delete_saved_games);
    //TODO: Print error message about invalid argument
}
