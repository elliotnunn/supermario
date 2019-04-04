# Introduction

The SuperMario sources (passed around as "System 7.1") are interesting
reading, but they cannot be built as-is. This project aims to produce
several modifications to SuperMarioProj ("patchsets") that can be
applied to the code and compiled into something useful.

You will need the SuperMarioProj sources to get started. These are not
hosted here, but a script is provided to import them from an HFS disk
image: `bin/getsrc System7.1.dmg`.

# Patching

Two Python scripts are provided to help use and modify the patchsets:
`bin/deepen` and `bin/flatten`. The first converts a folder of `.patch`
files to a branch in a git repository, and the second does the opposite.
Detailed help is available by running each with the `-h` switch.

By default, worktrees will be placed in `worktree/` (Git-ignored). I
suggest naming worktrees in lowercase to help avoid confusion with
patchset names, which are capitalised.

# Building

To build the sources, check out BuildCubeE (soon to be renamed).
