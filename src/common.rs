use std::path::PathBuf;

pub fn DIR () -> PathBuf {
    PathBuf::from(env!("HOME")).join(".tsudoku")//.to_str().unwrap().to_string()
}

pub mod csv {
    use std::{
        fs,
        io::Write,
    };
    use crate::terminal::display;

    pub fn read (filename: &str) -> Result<Vec<u8>, std::io::Error> {
        let mut data: Vec<u8> = std::fs::read(filename)?;
        data.retain(|&b| b != ',' as u8);   //NOTE: Strip commas from vector
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

