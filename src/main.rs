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

    let use_in_game_menu: bool =
        if matches.get_flag("no-in-game-menu") { true }
        else { false };
    let delete_saved_games: bool =
        if matches.get_flag("delete-saved-games") { true }
        else { false };
    println!("use_in_game_menu: {}", use_in_game_menu);
    println!("delete_saved_games: {}", delete_saved_games);
}
