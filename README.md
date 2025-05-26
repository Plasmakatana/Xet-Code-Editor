# Xet-Code-Editor
A lightweight and customizable Code Editor with RGB support written in pure C.
# DEMOS
On Ghostty
![Ghostty](https://github.com/Plasmakatana/Xet-Code-Editor/blob/main/xet-ghostty.png)
On Kitty
![Kitty](https://github.com/Plasmakatana/Xet-Code-Editor/blob/main/xet-kitty.png)
### C and Python support
![support](https://github.com/Plasmakatana/Xet-Code-Editor/blob/main/xet-py.png)
### Find feature
![search](https://github.com/Plasmakatana/Xet-Code-Editor/blob/main/xet-search.png)
# Installation
## For linux
  Install dependencies
  #### Arch
  ```bash
sudo pacman -S make
```
  #### Fedora
  ```bash
sudo dnf install make
```
  #### Debian/Ubuntu
  ```bash
sudo apt-get install make
```
  ### Git clone into directory
  ```bash
git clone https://github.com/Plasmakatana/Xet-Code-Editor.git
```
  ### Run the Makefile with 
  ```
  make
```
## For Windows
  -Install any 64-bit c/c++ compiler
  ### Git clone into any directory
  ```powershell
git clone https://github.com/Plasmakatana/Xet-Code-Editor.git
```
  ### Run the following commands depending on the compiler
  ```powershell
gcc -o Xet main.c -std=c99
```
  OR
  ```powershell
g++ -o Xet main.c
```
# Usage
  - After installation, either browse to the directory of installation
    or
    move the executable file to the desired location
  - Use standard commands 
    ```./Xet <args>``` on linux 
    OR
    ```.\Xet.exe <args>``` on windows 
    where "args" is the file-location of the file to be loaded 
    into the editor.
    Leave blank if creating new file.
  #### OR
  - Use the following command:
    ```<path/to/editor/Xet> <args>``` on linux
    OR
    ```<path\to\editor\Xet.exe> <args>``` on windows
  - You can also add ```<path/to/editor/Xet>``` to the
    ```/.bashrc``` or ```/.zshrc``` files on linux
    OR
    add ```<path\to\editor\>``` to ```PATHs``` on windows
## Shortcuts
  - Movement keys: &larr; &rarr; &uarr; &darr;
  - Large movement keys:<kbd>PgUp</kbd><kbd>PgDn</kbd><kbd>Home</kbd><kbd>End</kbd>
  - Find: <kbd>Ctrl</kbd>+<kbd>F</kbd>
  - Save: <kbd>Ctrl</kbd>+<kbd>S</kbd>
  - Quit: <kbd>Ctrl</kbd>+<kbd>Q</kbd> (twice in case of edited file)
