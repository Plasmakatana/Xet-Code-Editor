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
  #### Fedora/Redhat
  ```bash
sudo dnf install make
```
  #### CentOS/Redhat
  ```bash
sudo yum install make
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
  -Install MSYS2
  ### Git clone into any directory
  ```powershell
git clone https://github.com/Plasmakatana/Xet-Code-Editor.git
```
  ### Run the following commands depending on the compiler
  ```powershell
gcc -o Xet.exe main.c -std=c99
```
  OR
  ```powershell
g++ -o Xet.exe main.c
```
- Find all dependencies using:
```bash
ldd Xet.exe
```
- Move all dependencies to the same folder
## OR
- Download the windows release
- Extract the folder and start using the editor
# Usage
  - After installation, either browse to the directory of installation
    or
    move the executable file to the desired location
  - Use standard commands 
    ```./Xet filepath``` on linux 
    OR
    ```.\Xet.exe filepath``` on windows 
    where "filepath" is the file-location of the file to be loaded 
    into the editor.
    Leave blank if creating new file.
  #### OR
  - Use the following command:
    ```path/to/editor/Xet filepath``` on linux
    OR
    ```path\to\editor\Xet.exe filepath``` on windows
  - You can also add ```path/to/editor/Xet``` to the
    ```/.bashrc``` or ```/.zshrc``` files like so:
    ```zsh
    Xet(){
      path/to/executable/Xet "$@"
    }
    ```
    and then
    ```zsh
    source ~/.zshrc
    ```
    or
    ```bash
    source ~/.bashrc
    ```
    FOR WINDOWS
    add ```path\to\editor\``` to ```PATHs``` on windows
## Shortcuts
  - Movement keys: &larr; &rarr; &uarr; &darr;
  - Large movement keys:<kbd>PgUp</kbd><kbd>PgDn</kbd><kbd>Home</kbd><kbd>End</kbd>
  - Find: <kbd>Ctrl</kbd>+<kbd>F</kbd>
  - Save: <kbd>Ctrl</kbd>+<kbd>S</kbd>
  - Quit: <kbd>Ctrl</kbd>+<kbd>Q</kbd> (twice in case of edited file)
