# Introduction

The SuperMario sources (passed around as "System 7.1") are interesting
reading, but they cannot be built as-is. This project aims to produce
several modifications to SuperMarioProj ("patchsets") that can be
applied to the code and compiled into something useful.

# Requirements

- Python 3 (use pip to install the `macresources` and `machfs` packages)
- Mini vMac

# Hacking

To work with a patchset, it must be "deepened" into a worktree:

```
bin/deepen Custer custer-hacking
```

When the worktree destination contains no path separators, it will be
placed in `worktree/` (Git-ignored). I suggest naming worktrees in
lowercase to help avoid confusion with patchset names, which are
capitalised.

Hack on `worktree/custer-hacking` to your heart's content. When you are
ready to build, put the (escaped) path to your Mini vMac binary in
`bin/vmac_path.conf`, and run `./.build RISC` inside the worktree. This
helpful little wrapper passes your arguments to `bin/build`, which in
turn passes them to the MPW instance inside Mini vMac.

The commit history of a worktree matches the sequence of patch files in
a patchset. This system is intended to impose *some* structure on the
bewildering array of changes that have been made to the original
sources. These changes have been figured out by reversing released
binaries either newer or older than the source dump. They are grouped
according to function (e.g. "Sources", "Build system") rathen than
chronological order.

To preserve your changes to the commit structure, flatten them back into
the original patchset:

```
./.flatten
```

# Bugs

- Each patchset should have its own clear build instructions.
- There is no way to chain patchsets.

# Questions and Contributions

...Are very welcome! Open a GitHub PR/issue, or join us on our retro
Mac-hacking mailing list:

https://lists.ucc.gu.uwa.edu.au/mailman/listinfo/cdg5
