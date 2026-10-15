#!/bin/bash -e

# Linux build only
install_linux_deps() {
	local pkgs=(libirrlicht-dev cmake libsqlite3-dev libhiredis-dev \
		libgmp-dev gettext libpq-dev libleveldb-dev \
		libcurl4-openssl-dev zlib1g-dev)

	sudo apt-get update
	sudo apt-get install -y --no-install-recommends ${pkgs[@]} "$@"
}

# Mac OSX build only
install_macosx_deps() {
	brew update
	brew install freetype gettext hiredis irrlicht leveldb libogg libvorbis luajit
	if brew ls | grep -q jpeg; then
		brew upgrade jpeg
	else
		brew install jpeg
	fi
	#brew upgrade postgresql
}
