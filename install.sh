#!/bin/sh

set -e

install_app() {
    install -m775 ./miscount $1
}

if [[ $USER != "root" ]]; then
    echo -e "You are not root. Installing in ~/.local/bin\n"

    mkdir -p $HOME/.local/bin
    install_app $HOME/.local/bin

    echo -e "   Miscount is succesfully installed.\n"

    printf "Do you want this script to modify your PATH environment variable? [y/N] "
    read $ANS

    if [[ $ANS != 'y' ]]; then
        exit 0
    fi

    echo "export PATH=\$HOME/.local/bin:\$PATH" >> ~/.bashrc
    echo "export PATH=\$HOME/.local/bin:\$PATH" >> ~/.zshrc
    echo "export PATH=\$HOME/.local/bin:\$PATH" >> ~/.tcshrc

    echo "Miscount is now installed. Please run this command or restart your shell to start using Miscount."
    echo -e "   source $HOME/.bashrc"
    echo "NOTE: This script also added PATH exports for other POSIX shells, if any."
else
    install_app /usr/local/bin
    echo -e "\n   Miscount is succesfully installed.\n"
fi
