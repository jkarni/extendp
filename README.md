![Git log](https://raw.github.com/jkarni/extendp/master/static/gifs/menu-gitlog.gif)
# extendp - The pipe wildcard

Let's say you want to checkout someone git commit; you could tell which one it
is from the commit message, but you of course don't know its hash. So you type:

    git log

Find the commit, and then checkout it out with `git checkout <commit>`. This
pattern - using a command to get some information, and then using this
information to determine your next command - is common, but it's not entirely
ideal. After all, passing some information from one command to another (without
manually reading and inputing that information yourself) is exactly what pipes
are for; if we could use them in circumstances such as these, life would be
marginally better. But of course there's that little step in between that
requires the human touch: figuring out what commit you want.
 
**extendp** is the command for that human touch. It lets you see the output of
some command and pipe it along. Kind of like `tee`; but unlike tee, it lets you
see the output *before* deciding which command to pipe it to. There are two
fairly distinct modes, default and menu, but they both work on that shared
principle.

# Installation

    git clone https://github.com/jkarni/extendp.git
    cd extendp
    make && make install

(readline and glib are dependencies, so you might need to install those first:
`brew install glib readline` on OS X, for instace.)

# Usage

## Default mode

Without any options, `xp` prints its stdin to the screen (not, or not yet, to
stdout) and prompts the user for further commands. Those commands, which may be
multiple pipes combined, including further calls to xp, are passed the original
stdin, and then pass their stdout as `xp`'s stdout. That is:

    $ locate README | xp | xargs cat
    X| grep "mydir" 

(Where `$` represents input typed in a normal shell prompt, and `X|` represents
input typed at `xp`'s prompt) is equivalent to:

    $ locate README | grep "mydir" | xargs cat

But you get to see the output of `locate` first.

## Menu mode

Additionally, `xp` comes with a very useful "menu" option (`-m` or `--menu`).
This numbers the lines of `xp`'s input (much like shell's `select` command),
and prompts the user for one or more numbers. The lines corresponding to these
numbers alone are then passed to stdout.

### Menu with regex

Sometimes lines are not the most sensible units for filtering. For instance,
you might want to do something like this:

    $ git log | xp [?] | xargs git checout
    X| 5
    ==> $ git checkout <fifth commit hash in "git log">

To make this easy, `xp` allows you to use named regexes. In HOME/.extendp.cfg,
the following:

    [gitlog]
    regex=^commit ([[:digit:]abcdef]*)$

Allows you to use:

    $ git log | xp --menu=gitlog [or --m=gitlog] | xargs git checkout

To get something like this:

![git log examples](/static/gitlog.png)


# Contributions

I suck at C, and doubt there's much here that couldn't stand improving; pull
requests are, therefore, particularly encouraged.
