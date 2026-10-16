# Contributing to SquareOne Core

SquareOne Core is dedicated server software that preserves compatibility with
supported Minetest 0.4 and 5.x clients. Contributions should keep that server
and protocol focus; this repository does not maintain a graphical client.

## Before coding

Search the [existing issues](https://github.com/monk-afk/squareone-core/issues)
first. For substantial behavior, protocol, persistence, or compatibility
changes, open an issue before implementation so the design and migration risks
can be discussed.

Security vulnerabilities must be reported privately as described in
[SECURITY.md](SECURITY.md), not through a public issue.

## Preparing a change

1. Create a focused branch from `main`.
2. Keep unrelated changes out of the branch.
3. Preserve the established server, Lua API, world-format, and dual-protocol
   compatibility contracts unless the change explicitly documents otherwise.
4. Add or update tests and documentation where the behavior changes.
5. Use concise, present-tense commit subjects. Prefer small reviewable commits
   for mechanical or architectural work.

For a normal debug build and unit test run:

```sh
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DRUN_IN_PLACE=TRUE \
    -DBUILD_UNITTESTS=TRUE
cmake --build build -j2
./bin/squareoneserver --run-unittests
```

The C++ formatting check follows clang-format 11. Do not apply a newer
clang-format version across existing source files; its output can differ from
the CI requirement.

Run `git diff --check` before submitting a change. Where practical, also test
the affected behavior with a temporary world and compatible clients.

## Pull requests

Open the pull request against
[`main`](https://github.com/monk-afk/squareone-core/compare). Include:

- the problem and intended outcome;
- the implementation and compatibility impact;
- exact test commands and results;
- any behavior that could not be tested locally;
- migration, persistence, or deployment considerations, when applicable.

A passing workflow is required but does not replace review of protocol,
persistence, and Lua compatibility consequences.
