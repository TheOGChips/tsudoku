use std::path::PathBuf;

//TODO: Replace these anywhere I haven't yet
pub const NUMERIC_DATA_FILENAME: &str = "numbers.csv";
pub const COLOR_DATA_FILENAME: &str = "colors.csv";

pub fn DIR () -> PathBuf {
    PathBuf::from(env!("HOME")).join(".tsudoku")//.to_str().unwrap().to_string()
}

pub mod csv {
    use std::{
        fs,
        io::Write,
    };
    use crate::{
        terminal::display,
        common,
    };

    //TODO: Just go ahead and have this parse the newlines out and return a 2D vector
    pub fn read (filename: &str) -> Result<Vec<u8>, std::io::Error> {
        display::tui_end();
        let data: Vec<u8> = 
            if filename == common::COLOR_DATA_FILENAME {
                //NOTE: If reading color data, simply read and strip commas from vector
                let mut data: Vec<u8> = std::fs::read(filename)?;
                data.retain(|&b| b != ',' as u8);
                data
            }
            else {
                let data_string: String = std::fs::read_to_string(filename).expect(
                    format!("Couldn't find file {} to read from...", filename).as_str()
                );
                let mut data: Vec<u8> = Vec::new();
                for line in data_string.split('\n') {
                    println!("{:?}", line);
                    if !line.is_empty() {
                        let mut line: Vec<u8> = line.split(',').map(
                            |s| s.parse().expect(
                                format!("Expected to parse a number from {}", filename).as_str()
                            )
                        ).collect();
                        line.push('\n' as u8);
                        data = [data, line].concat();
                    }
                }
                data
            };
        for b in data {
            if b == '\n' as u8 {
                println!("");
            }
            else {
                print!("{} ", b);
            }
        }
        std::process::exit(1);
        Ok(data)
    }

    pub fn write<T: ToString> (
        save_game_name: &str,
        data_file_name: &str,
        data: &Vec<[T; display::DISPLAY_MATRIX_ROWS]>) {
            let save_dir = crate::DIR().join(&save_game_name);
            let _ = fs::create_dir(&save_dir);
            let mut outfile: fs::File = fs::OpenOptions::new().create(true)
                .truncate(true)
                .write(true)
                .open(save_dir.join(data_file_name))
                .expect(format!("Unable to create or open {}", data_file_name).as_str());
            for d in data {
                let d: String = d.iter()
                    .map(|item| item.to_string())
                    .collect::<Vec<String>>()
                    .join(",");
                outfile.write_all(d.as_bytes())
                    .expect(format!("Unable to write to {}...", data_file_name).as_str());
                outfile.write_all(b"\n")
                    .expect(
                        format!("Unable to write newline for {}...", data_file_name
                    ).as_str());
            }
    }
}

