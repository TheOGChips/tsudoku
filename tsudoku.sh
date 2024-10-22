bin_name=$(basename "$0")
tgt=$(basename "$0" | awk -F '.' '{print $1}')
sleep_time=2

function help_menu {
    echo "
Usage: $bin_name <OPTION>

Valid OPTIONs:

    --help    -> Display this help menu
    install   -> Install $tgt and dependencies (requires sudo priveleges)
    upgrade   -> Upgrade $tgt
    uninstall -> Uninstall $tgt
    purge     -> Uninstall $tgt and remove all game data
"
}

function install_deps {
    for mgr in apt yum dnf pkg zypper
        do which $mgr &> /dev/null
        if [ "$?" -eq 0 ]
            then pkg_mgr=$mgr
        fi
    done

    pkgs=()
    cmds=("$pkg_mgr")
    if [ "$pkg_mgr" = 'apt' ]
        then ncurses_pkg=libncurses-dev
        cmds+=(list)
        cmds+=(--installed)
    
    elif [ "$pkg_mgr" = 'dnf' ] || [ "$pkg_mgr" = 'yum' ]
        then ncurses_pkg=ncurses-devel
        cmds+=(list)
        cmds+=(installed)
    
    elif [ "$pkg_mgr" = 'pkg' ]
        then ncurses_pkg=ncurses
        cmds+=(info)

    elif [ "$pkg_mgr" = 'zypper' ]
        then ncurses_pkg=ncurses-devel
        cmds+=(search)
        cmds+=(--installed-only)

    else echo "
Error: Unknown type of package manager. Unable to install missing required
       packages. Submit a bug fix to add your package manager by emailing the
       author of $tgt.
"
        exit 1
    fi

    "${cmds[@]}" "$ncurses_pkg" | grep "$ncurses_pkg" &> /dev/null
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
        sleep $sleep_time
        sudo $pkg_mgr update
        sudo $pkg_mgr install -y "${pkgs[@]}"
    fi

    # Check if Rust and Cargo are already installed and skip if it is
    cargo --version &> /dev/null
    if [ "$?" -ne 0 ]
        then echo '
NOTE: You are missing Rust and Cargo. Attempting to install them now...
'
        sleep $sleep_time
        curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
        source "$HOME"/.cargo/env
    fi
}

function install {
    cargo install $tgt
}

function upgrade {
    uninstall
    install
}

function uninstall {
    cargo uninstall $tgt
}

function purge {
    uninstall
    rm -rf "$HOME"/.$tgt
}

if [ "$#" -ne 1 ]
    then echo -e '\nError: One and only one argument is required'
    help_menu

elif [ "$1" = '--help' ]
    then help_menu

elif [ "$1" = 'install' ]
    then install_deps
    echo "
Installing $tgt...
"
    sleep $sleep_time
    install
    echo "
Reminder: You might need to source your shell's RC file (e.g. .bashrc or .zshrc)
          before you can run '$tgt' if you're installing it for the first time.
          You can do so via 'source ~/.bashrc', etc.
"

else which $tgt &> /dev/null
    if [ "$?" -ne 0 ]
        then echo "
Error: Cannot $1 $tgt. It doesn't appear to be installed.
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
fi
