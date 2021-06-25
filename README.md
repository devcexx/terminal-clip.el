# terminal-clip.el

This plugin is a simple Emacs plugin that allows to interact with the
system clipboard when running Emacs without Window System support
(e. g when running Emacs from the terminal).

This little plugin is an alternative to
[xclip](https://github.com/emacsmirror/xclip/blob/master/xclip.el),
but using an Emacs dynamic module, that relies on the
[clip](https://github.com/dacap/clip) library for performing all the
copy/paste logics, instead of executing programs like `xcopy` or
`pbcopy` for doing so. By doing this, we avoid the necessity of
spawning a new process and waiting for its completion each time the
user wants to copy or paste something from or to the clipboard.

Additionally, this plugin is not intended to define new logics for
kill and yank commands, nor new keybindings for interacting with the
clipboard in a special way.  Instead, the idea behind it is just to
port the clipboard functionality that Emacs does by default when
running on a windowed environment to a terminal environment.

## Installation

Steps:

Clone this repo into your .emacs.d folder:

```bash
git clone --recursive https://github.com/devcexx/terminal-clip.el
```

Compile the dynamic module:

```bash
cd terminal-clip.el
cmake .
cmake --build .
```

Edit your `init.el` and add the following code for loading the plugin:

```elisp 
(add-to-list 'load-path (concat user-emacs-directory "terminal-clip.el")) ;; Important, add the repository folder into the load-path!
(require 'terminal-clip)
(when terminal-clip-available ;; Enable only when available (no running with a window system, Emacs compiled with module support...)
	(terminal-clip-mode t))
```

## Troubleshooting

### CMake cannot find 'emacs-module.h'

The building process requires the Emacs headers. If your Emacs
distribution doesn't come with them, you may download them and tell
CMake where they are manually:

1. Clean the CMake workspace by removing the CMakeCache.txt and
   CMakeFiles entries.
1. Clone the Emacs repo from https://github.com/emacs-mirror/emacs
   somewhere in your computer and checkout your current version.
1. Run CMake like this:

```bash
cd <path-to-the-clone-of-this-repo>
CXXFLAGS=-I<path-to-emacs-sources>/src cmake .
cmake --build .
```

### CMake generates a '.so' file instead of a '.dylib' file in macOS.

That might be actually normal. In my personal Emacs installation, for
some reason, it expects the module to have the extension '.so'. So
yeah, can be normal.

### CMake says 'execute_process error getting child return code: No such file or directory'

This is most likely produced because CMake is trying to ask Emacs what
should be the name of the final modules, but the Emacs executable is
not in the PATH. For fixing that, just clean the CMake cache, and then
retrying execting instead the command: `cmake -DEMACS_EXEC_PATH=<path-to-emacs-executable> .`
