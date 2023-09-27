fn main() {
    //cxx_build::CFG.include_prefix = "include/";

    cxx_build::bridge("src/main.rs")
        .include("include/")
        .file("src/tsudoku.cpp")
        .file("src/misc.cpp")
        .file("src/Menu.cpp")
        .file("src/MainMenu.cpp")
        .file("src/SavedGameMenu.cpp")
        .file("src/DifficultyMenu.cpp")
        .file("src/InGameMenu.cpp")
        .file("src/Sudoku.cpp")
        .file("src/Grid.cpp")
        .file("src/Container.cpp")
        .flag_if_supported("-std=c++17")
        .compile("tsudoku");
}
