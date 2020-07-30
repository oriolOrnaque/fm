# fm
Minimal terminal file manager with ncurses.
Based on [nnn](https://github.com/jarun/nnn).

![main screen](https://raw.githubusercontent.com/oriolOrnaque/fm/master/imgs/capture1.png)

## Controls
Vim-like navigation:
* **h** left
* **j** down
* **k** up
* **l** right
* **G** jump to first entry
* **g** jump to last entry

Exit:
* **q** exit changing your cwd to the last visited directory
* **ESC** exit without changing directories

Selection:
* **space** toggle selection

View:
* **h** show hidden files
* **r** redraw the view

Files:
* **n** create new file or directory. To create a directory just append '/' to the name.
* **d** delete all the selected files or directories
