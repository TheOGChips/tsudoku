use std::{
    env,
    path::PathBuf,
    process::Command,
};
use regex::Regex;

//TODO: Read these in using the fs crate instead
const BASENAMES: [&str; 10] = ["tsudoku", "misc", "Menu", "MainMenu", "SavedGameMenu", "DifficultyMenu",
                               "InGameMenu", "Sudoku", "Grid", "Container"];

fn main() {
    let src_path = PathBuf::from("src") //The directory for source (*.cpp) files
        .canonicalize()
        .expect("Cannot canonicalize src path...");
    let include_path = PathBuf::from("include") //The directory for header (*.hpp) files
        .canonicalize()
        .expect("Cannot canonicalize include path...");
    //NOTE: Cannot be be canonicalized because the path won't yet exist at this point.
    let lib_path: &str = "target/lib";
    let _ = std::fs::create_dir_all(lib_path);
    let lib_path = PathBuf::from("target/lib")
        .canonicalize()
        .expect("Cannot canonicalize lib path...");
    let lib = lib_path.join("libtsudoku.a");
    //let srcs = BASENAMES.map(|basename| src_path.join(basename).to_str().unwrap().to_owned());
    let headers = BASENAMES.map(
        |basename| include_path.join(basename.to_owned() + ".hpp")
            .to_str()
            .unwrap()
            .to_owned()
    );

    println!("cargo:rustc-link-search={}", lib_path.to_str().unwrap());
    println!("cargo:rustc-link-lib=static=tsudoku");
    println!("cargo:rerun-if-changed={}", headers.join(","));
    println!("cargo:rustc-link-lib=dylib=stdc++");
    println!("cargo:rustc-link-lib=dylib=ncurses");

    for src in BASENAMES.iter().filter(|name| !name.contains("Menu")) {
        if !Command::new("clang++")
            .arg("-c")
            .arg("-o")
            .arg(lib_path.join(src.to_string() + ".o"))
            .arg("-lncurses")
            .arg(src_path.join(src.to_string() + ".cpp"))
            .arg("-I")
            .arg(&include_path)
            .arg("-std=c++17")
            //.output()
            .status()
            .expect("Could not spawn `clang++`")
            //.status
            .success() {
                panic!("Could not compile non-Menu files...");
            }
    }

    if !Command::new("clang++")
        .arg("-c")
        .arg("-o")
        .arg(lib_path.join("Menu.o"))
        .arg("-lncurses")
        .arg(src_path.join("Menu.cpp"))
        .arg("-I")
        .arg(&include_path)
        .arg("-std=c++17")
        .output()
        .expect("Could not spawn `clang++`")
        .status
        .success() {
            panic!("Could not compile Menu.cpp...");
        }

    for src in BASENAMES.iter().filter(|name| Regex::new(r".+Menu").unwrap().is_match(name)) {
        if !Command::new("clang++")
            .arg("-c")
            .arg("-o")
            .arg(lib_path.join(src.to_string() + ".o"))
            .arg("-lncurses")
            .arg(src_path.join(src.to_string() + ".cpp"))
            .arg("-I")
            .arg(&include_path)
            .arg("-std=c++17")
            //.output()
            .status()
            .expect("Could not spawn `clang++`")
            //.status
            .success() {
                panic!("Could not compile Menu classes...");
            }
    }

    for obj in BASENAMES {
        if !Command::new("ar")
            .arg("rcs")
            .arg(lib.clone())
            .arg(lib_path.join(obj.to_string() + ".o"))
            //.output()
            .status()
            .expect("Could not spawn `ar`")
            //.status
            .success() {
                panic!("Could not emit library file...");
            }
    }

    let bindings = bindgen::Builder::default()
        .opaque_type("std::.*") //NOTE: Causes a warning to appear when left on for some reason
        .allowlist_type("MainMenu")
        .allowlist_type("Sudoku")
        .allowlist_type("SavedGameMenu")
        .allowlist_type("cell")
        .allowlist_function("display_completed_puzzles")
        .header(include_path.join("tsudoku.hpp").to_str().unwrap())
        .header(include_path.join("misc.hpp").to_str().unwrap())
        .header(include_path.join("Menu.hpp").to_str().unwrap())
        .header(include_path.join("MainMenu.hpp").to_str().unwrap())
        .header(include_path.join("SavedGameMenu.hpp").to_str().unwrap())
        .header(include_path.join("DifficultyMenu.hpp").to_str().unwrap())
        .header(include_path.join("InGameMenu.hpp").to_str().unwrap())
        .header(include_path.join("Sudoku.hpp").to_str().unwrap())
        .header(include_path.join("Grid.hpp").to_str().unwrap())
        .header(include_path.join("Container.hpp").to_str().unwrap())
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        .generate()
        .expect("Unable to generate bindings");
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap()).join("bindings.rs");
    bindings.write_to_file(out_path)
        .expect("Couldn't write bindings!");
}
