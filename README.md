# Xet-Code-Editor
A lightweight and customizable Code Editor with RGB support written in pure C.
# DEMOS
On Ghostty
![Ghostty](https://github.com/Plasmakatana/Xet-Code-Editor/blob/main/xet-ghostty.png)
On Kitty
![Kitty](https://github.com/Plasmakatana/Xet-Code-Editor/blob/main/xet-kitty.png)
# C and Python support
![support](https://github.com/Plasmakatana/Xet-Code-Editor/blob/main/xet-py.png)
# Search feature
![seacrh](https://github.com/Plasmakatana/Xet-Code-Editor/blob/main/xet-search.png)
# Installation
## For linux
  Install dependencies
  #### Arch
  ```sudo pacman -S make```
  #### Fedora
  ```sudo dnf install make```
  #### Debian/Ubuntu
  ```sudo apt-get install make```
  ### Git clone into directory
  ```git clone https://github.com/Plasmakatana/Xet-Code-Editor.git```
  ### Run the Makefile with 
  ```make```
## For Windows
  -Install any 64-bit c/c++ compiler
  ### Git clone into any directory
  ```git clone https://github.com/Plasmakatana/Xet-Code-Editor.git```
  ### Run the following commands depending on the compiler
  ```gcc -o Xet main.c -std=c99```
  OR
  ```g++ -o Xet main.c```
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
  ## OR
  - Use the following command:
    ```<path/to/editor/Xet> <args>``` on linux
    OR
    ```<path\to\editor\Xet.exe> <args>''' on windows
  - You can also add <path/to/editor/Xet> to the
    /.bashrc or /.zshrc files on linux
    OR
    add <path\to\editor\> to PATHs on windows
