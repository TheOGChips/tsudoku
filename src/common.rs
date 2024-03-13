use std::path::PathBuf;

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

pub fn dbgprint (msg: &str) {
    ncurses::clear();
    ncurses::mvprintw(10, 10, msg);
    ncurses::refresh();
    ncurses::getch();
}
