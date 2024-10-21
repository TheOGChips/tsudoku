bin_name=$(basename "$0")

function help_menu {
    echo "
Usage: $bin_name <OPTION>

Valid OPTIONs:

    install   -> Install tsudoku and dependencies (requires sudo priveleges)
    upgrade   -> Upgrade tsudoku
    uninstall -> Uninstall tsudoku
    purge     -> Uninstall tsudoku and remove all game data
"
}

function install_deps {
    for mgr in apt dnf yum
        do which $mgr
        if [ "$?" -eq 0 ]
            then pkg_mgr=$mgr
        fi
    done

    pkgs=()
    if [ "$pkg_mgr" = 'apt' ]
        then ncurses_pkg=libncurses-dev
        apt list --installed "$ncurses_pkg" | grep "$ncurses_pkg" &> /dev/null
    
    elif [ "$pkg_mgr" = 'dnf' ] || [ "$pkg_mgr" = 'yum' ]
        then ncurses_pkg=ncurses-devel
        "$pkg_mgr" list installed "$ncurses_pkg" | grep "$ncurses_pkg" &> /dev/null
    
    else echo '
Error: Unknown type of package manager. Unable to install missing required
       packages. Submit a bug fix to add your package manager by emailing the
       author of tsudoku.
'
        exit 1
    fi

    if [ "$?" -ne 0 ]
        then pkgs+=("$ncurses_pkg")
    fi

    for pkg in gcc curl
        do $pkg --version &> /dev/null
        if [ "$?" -ne 0 ]
            then pkgs+=($pkg)
        fi
    done
    
    if [ "${#pkgs[@]}" -gt 0 ]
        then echo "
NOTE: You are missing the following prerequisite packages: ${pkgs[@]} . Attempting to install
      them now...
"
        sudo $pkg_mgr update
        sudo $pkg_mgr install "${pkgs[@]}"
    fi

    # Check if Rust and Cargo are already installed and skip if it is
    cargo --version &> /dev/null
    if [ "$?" -ne 0 ]
        then echo '
NOTE: You are missing Rust and Cargo. Attempting to install them now...
'
        curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
        source "$HOME"/.cargo/env
    fi
}

function install {
    cargo install tsudoku
}

function upgrade {
    uninstall
    install
}

function uninstall {
    cargo uninstall tsudoku
}

function purge {
    uninstall
    rm -rf "$HOME"/.tsudoku
}

if [ "$#" -ne 1 ]
    then echo -e '\nError: One and only one argument is required'
    help_menu

elif [ "$1" = '--help' ]
    then help_menu

elif [ "$1" = 'install' ]
    then install_deps
    install
    echo "
Reminder: You might need to source your shell's RC file (e.g. .bashrc or .zshrc)
          before you can run 'tsudoku' if you're installing it for the first time.
          You can do so via 'source ~/.bashrc', etc.
"

elif [ "$1" = 'upgrade' ]
    then upgrade

elif [ "$1" = 'uninstall' ]
    then uninstall

elif [ "$1" = 'purge' ]
    then purge

else echo -e '\nError: Invalid argument.'
    help_menu
fi
