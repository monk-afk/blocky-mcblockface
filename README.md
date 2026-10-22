# Blocky McBlockface

![Build Status](https://github.com/monk-afk/blocky-mcblockface/actions/workflows/build.yml/badge.svg)
[![License](https://img.shields.io/badge/license-LGPLv3.0%2B-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0.en.html)
[![License: CC BY-SA 3.0](https://img.shields.io/badge/license-CC_BY--SA_3.0-orange.svg)](https://creativecommons.org/licenses/by-sa/3.0/)

Blocky McBlockface is server software for hosting Minetest-compatible voxel
worlds. It maintains the dual-protocol support inherited from MultiCraft 2.0.6,
allowing supported Minetest 0.4 and 5.x clients to connect to the same server.

This repository builds only the `mcblockface` executable. It does not contain
or build a graphical client. `BUILD_CLIENT=ON` is rejected at configuration
time so a package cannot accidentally advertise a client that Blocky
McBlockface no longer maintains.

Blocky McBlockface is based on
[MultiCraft 2.0.6](https://github.com/MultiCraft/MultiCraft/releases/tag/2.0.6),
which was based on
[Minetest 5.4.1](https://github.com/luanti-org/luanti/tree/5.4.1).

## Documentation

- [Lua server API](doc/lua_api.txt)
- [Network protocol](doc/protocol.txt)
- [World format](doc/world_format.txt)
- [Server texture overrides](doc/texture_packs.txt)
- [Server-sent client Lua API](doc/client_lua_api.txt)
- [mcblockface(6)](doc/mcblockface.6)

The client Lua document remains because the server can distribute restricted
client-side code through SSCSM. It is not a bundled-client API.

## Building on GNU/Linux

Required development dependencies include a C++ compiler, CMake, Irrlicht
headers, SQLite, zlib, GMP, JSONCPP, and Lua or LuaJIT. cURL, gettext, ncurses,
PostgreSQL, LevelDB, Redis, Prometheus, and SpatialIndex support are optional.

A typical Debian or Ubuntu development environment can be prepared with:

```sh
sudo apt install build-essential cmake libirrlicht-dev libsqlite3-dev \
    zlib1g-dev libgmp-dev libjsoncpp-dev libluajit-5.1-dev \
    libcurl4-openssl-dev libncurses-dev gettext
```

Clone and build a run-in-place server:

```sh
git clone https://github.com/monk-afk/blocky-mcblockface.git
cmake -S blocky-mcblockface -B blocky-mcblockface/build \
    -DCMAKE_BUILD_TYPE=Release \
    -DRUN_IN_PLACE=TRUE
cmake --build blocky-mcblockface/build -j2
./blocky-mcblockface/bin/mcblockface --help
```

For an installed layout, configure with `-DRUN_IN_PLACE=FALSE` and use
`cmake --install blocky-mcblockface/build` after building.

Irrlicht is still required for shared data types and server-side serialization
code. No renderer, GUI, audio engine, or graphical executable is built.

## Tests

Enable and run the C++ unit tests with:

```sh
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DRUN_IN_PLACE=TRUE \
    -DBUILD_UNITTESTS=TRUE
cmake --build build -j2
./bin/mcblockface --run-unittests
```

Use `cmake -LAH -N build` to inspect the available build options.

The supported continuous-integration targets are GCC, Clang, Docker, and
32-bit and 64-bit Visual Studio server builds.

## Running

Select a world and game explicitly:

```sh
./bin/mcblockface \
    --world /path/to/world \
    --gameid <game-id> \
    --config /path/to/mcblockface.conf
```

Use `--terminal` when the server was built with ncurses support and an
interactive console is wanted. Run `mcblockface --help` for the complete
command-line reference.

## Configuration and paths

The primary configuration filename is `mcblockface.conf`. For compatibility,
`minetest.conf` is accepted only when `mcblockface.conf` does not exist in the
same search locations. An explicit `--config` path takes precedence.

Installed Unix builds use these defaults:

- executable: `/usr/bin/mcblockface`
- shared data: `/usr/share/mcblockface`
- user data: `~/.mcblockface`
- worlds: `~/.mcblockface/worlds/`

Run-in-place builds use the source tree for their binary, shared data, and user
data locations.

Server-list announcements retain `server_id = "minetest"` for compatibility.
No MultiCraft configuration directory or executable alias is used.

The complete commented configuration reference is
[mcblockface.conf.example](mcblockface.conf.example).

## Docker

Build the server image with:

```sh
docker build -t blocky-mcblockface .
```

The image runs as an unprivileged `mcblockface` user, stores runtime data under
`/var/lib/mcblockface`, and exposes UDP port 30000. When Prometheus is enabled,
the configured metrics listener also uses TCP.

## Versioning

Blocky McBlockface releases use semantic version numbers. A release tag matches
the version reported by `mcblockface --version`; development identifiers are
not appended to stable release numbers.

## Repository

Source, issues, and releases are hosted at:

https://github.com/monk-afk/blocky-mcblockface
