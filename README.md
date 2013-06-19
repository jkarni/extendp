<link href="http://kevinburke.bitbucket.org/markdowncss/markdown.css" rel="stylesheet"></link>
# extendp - The pipe wildcard

Pipes are great, but they're often a bit too inflexible. Suppose you want to
open a file named `README`; you're not sure where it is, so you type:

    find / -name "README.*"

Wow - way too many lines of "Permission denied" to make sense of. Let's try
again:

    find / -name "README.*" 2>/dev/null

Still too many results. So you limit the search to your home folder:

    find $HOME -name "README.*" 2>/dev/null

And still there are too many. Despite your familiarity with your shell's vi or
emacs bindings, typing these commands in each time is getting annoying, as is waiting
for `find` to run again and again. If only you could interactively filter,
seeing the output so far, manipulating it with, say `grep`, and finally using
it with `open`.

If that's the case, you could try this:

    find / -name "README.*" | xp | xargs open

But you'll need this first:


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


