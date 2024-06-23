use std::path::PathBuf;
use pancurses as pc;

pub fn DIR () -> PathBuf {
    PathBuf::from(env!("HOME")).join(".tsudoku")//.to_str().unwrap().to_string()
}

pub mod csv {
    pub fn read (filename: &str) -> Result<Vec<u8>, std::io::Error> {
        let mut data: Vec<u8> = std::fs::read(filename)?;
        data.retain(|&b| b != ',' as u8);   //NOTE: Strip commas from vector
        Ok(data)
    }

    pub fn write (data: Vec<u8>) {}
}

pub fn dbgprint (window: &pc::Window, msg: &str) {
    window.clear();
    window.mvprintw(10, 10, msg);
    window.refresh();
    window.getch();
}
