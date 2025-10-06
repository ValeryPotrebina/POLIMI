---
marp: true
---

# **An introduction to Bash**

## Moving quickly in the Terminal
It should almost never happen that you have to write a terminal command by typing it out character by character. Instead, you should rely on shortcuts to be more efficient. Here some tips:

- `Tab`: Auto-completes commands and paths. E.g. if in the current folder there are the files `main.cpp` and `test.cpp`, if you type `g++ m` and press `Tab`, it will auto-complete to `g++ main.cpp`.
- `Up` and `Down` arrows: Slide through previous commands.
- `Ctrl + R`: Start a incremental reverse search of your bash history. In other words it finds the last complete command that you executed that starts with what you are typing.
- `Ctrl + L`: Similar to `clear` command, clears the terminal screen


### Basic Bash Commands 
- `echo`: returns whatever you type at the shell prompt similar to `print` in Python, or `disp` in Matlab.
- `date`: displays the current time and date.
- `clear`: clean the terminal.
- `pwd` stands for **Print working directory** and it points to the current working directory, that is, the directory that the shell is currently looking at. It’s also the default place where the shell commands will look for data files.
- `ls` stands for a **List** and it lists the contents of a directory. ls usually starts out looking at our home directory. This means if we print ls by itself, it will always print the contents of the current directory.
- `cd` stands for **Change directory** and changes the active directory to the path specified.
- `mkdir` stands for **Make directory** and is used to make a new directory or a folder.
- `mv` stands for **Move** and it moves one or more files or directories from one place to another. We need to specify what we want to move, i.e., the source and where we want to move them, i.e., the destination.
- `touch` command is used to create new, empty files. It is also used to change the timestamps on existing files and directories.
- `rm` stands for **Remove** and it removes files or directories. By default, it does not remove directories, but if used as `rm -r *` within a directory, then every directory and file inside that directory is deleted (`*` is a special characters that matches everything).
- `env` list the environmental variables

---

### Run a script
To run your brand new script you may need to change the access permissions of the file. To make a file executable run

```bash
chmod +x script_file
```

---

### Wildcards
A wildcard is a character (or set of characters) that in a command matches one or more characters. It is really useful for searching a file (maybe you know just its extension) or applying a command to a subset of files (maybe you want to copy only files starting with `2023-`).

- The wildcard `?` matches a single character. E.g., `S??n` will match anything that begins with S and end with n and has exactly two characters between them. It matches `Soon` and `Sean` but not `Sin`. 
- The wildcard `*` matches any number of characters or a set of characters. E.g., `S*n` will match anything between S and n. The number of characters between them does not count. It matches `Soon`, `Sean` and `Sin`.
- The wildcard `[]` matches characters that are enclosed in the square braces. E.g., `S[on]n` matches only `Son` and `Snn`. We can also specify range of characters with the `-` character. E.g., braces like `S[a-d]n` match `San`, `Sbn`, `Scn`, `Sdn`.

---

### <span style="color:blue;">Exercises (solutions)</span>
- Go to your home folder (*Suggestion:* you can either use `~` or `$HOME`)

```cd $HOME```

- Create a folder named `test1`

```mkdir test1```

- Go inside `test1` and create a directory `test2`

```cd test1 && mkdir test2```

- Go inside `test2` and then up one directory (*Suggestion:* `..` indicates the upper directory)

```
cd test2 && ls
cd ..
```

- Create the following files `f1.txt`, `f2.txt`, `f3.dat`, `f4.md`, `readme.md`, `.hidden`

```touch f1.txt f2.txt f3.dat f4.md readme.md .hidden```

- List all files in the directory, also the hidden ones

```ls -a```

- List only files with txt extension (*Suggestion:* use `*` wildcard)

```ls -a *.txt```

- List files with `1`, `2`, `3` or `4` in the name (*Suggestion:* use `[1-4]` wildcard)

```ls *[1-4].*```

- Move the `readme.md` in `test2`

```mv readme.md test2```

- Move all txt files in `test2` in one command

```mv *.txt test2```

- Remove `f3.dat`

```rm f3.dat```

- Remove all contents of `test2` and the folder itself in one commands

```rm -r test2```

- Remove all contents of `test1` and the folder itself in one commands

```cd .. && rm -r test1```

---

### Download and extract a Matrix
With `wget` you can retrieve content from web servers. For instance, you can download a matrix from the matrix market with 

```wget https://math.nist.gov/pub/MatrixMarket2/NEP/mhd/mhd416a.mtx.gz```

To unzip the file, simply type `gzip -dk mhd416a.mtx.gz`

---

### More commands
- `cat` stands for **Concatenate** and it reads a file and outputs its content. It can read any number of files, and hence the name concatenate.
- `wc` is short for **Word count**. It reads a list of files and generates one or more of the following statistics: newline count, word count, and byte count.
- `grep` stands for **Global regular expression print**. It searches for lines with a given string or looks for a pattern in a given input stream.
- `head` show the first lines of a file
- `tail` show the last lines of a file 
- `file` reads the files specified and performs a series of tests in attempt to classify them by type

---

### Redirection, Pipelines and Filters
We can add operators between commands in order to chain them together.
- The pipe operator `|`, forwards the output of one command to another. E.g. `cat /etc/passwd | grep user` checks system information about "user".
- The redirect operator `>` sends the standard output of one command to a file. E.g. `ls > files-in-this-folder.txt` saves a file with the list of files.
- The append operator `>>` appends the output of one command to a file.
- The operator `&>` sends the standard output and the standard error to file
- `&&`  pipe is activated only if the return status of the first command is 0. It is used to chain commands together: e.g. `sudo apt update && sudo apt upgrade`
- `||` pipe is activated only if the return status of first command is different from 0.
- `;` is a way to execute to commands regardless of the output status
- `$?` is a variable containing the output status of the last command

---

### <span style="color:blue;">Exercises (solutions)</span>
- Create a file with the current date and time (one command) and display its content

 ```date > date.txt && cat date.txt```

- Count the number of lines in the file `mhd416a.mtx` (*Suggestion:* use `cat`, `wc` and `|`)

 ```
 wc -l mhd416a.mtx
 cat mhd416a.mtx | wc -l
 ```

- List the entries of the matrix that are smaller than 1e-9 in absolute value. You can assume that all values are in exponential format and all values are greater than 1e-100 in absolute value. Count how many entries satisfy this criteria. (*Suggestion:* use `cat`, `grep`, `wc` and `|` )
```
cat mhd416a.mtx | grep e-[1-9][0-9]
cat mhd416a.mtx | grep e-[1-9][0-9] | wc -l
```

- Find and count the lines with entries smaller than 1e-15 in the matrix file `mhd416a.mtx`
```
cat mhd416a.mtx | grep e-1[6-9] && cat -n mhd416a.mtx | grep e-[2-9][0-9] 
cat -n mhd416a.mtx | grep e-1[6-9] | cut -f 1 > lines.txt
cat -n mhd416a.mtx | grep e-[2-9][0-9] | cut -f 1 >> lines.txt
wc -l lines.txt
```

- Remove the lines with entries smaller than 1e-15 from the matrix file `mhd416a.mtx`
```
sed -i '/e-1[6-9]/d' mhd416a.mtx 
sed -i '/e-[2-9][0-9]/d' mhd416a.mtx

cat -n mhd416a.mtx | grep e-1[6-9] && cat -n mhd416a.mtx | grep e-[2-9][0-9]
```
N.B. To match the `.mtx` format remember to modify the third line in the matrix file consistently with the number of lines removed: 8562 -> 8508


### Advanced commands - `sed`, `cut` and `find` <span style="color:red;float:right">(Advanced)</span>
 - `sed` stands for **stream editor** and it can perform lots of functions on file like searching, find and replace, insertion or deletion. We give just an hint of its true power
    - `echo "unix is great os. unix is open source." | sed 's/unix/linux/'` replaces the first occurrence of "unix" with "linux"
    - `echo "unix is great os. unix is open source." | sed 's/unix/linux/2'` replaces the second occurrence of "unix" with "linux"
    - `echo "unix is great os. unix is open source." | sed 's/unix/linux/g'` replaces all occurrences of "unix" with "linux"
    - `echo -e "ABC\nabc" | sed '/abc/d'` delete a line matching "abc"
    - `echo -e "1\n2\n3\n4\n5\n6\n7\n8" | sed '3,6d'` delete lines 3 to 6

 - `cut` is a command for cutting out the sections from each line of files and writing the result to standard output.
     - `cut -b 1-3,7- state.txt` cut bytes (`-b`) from 1 to 3 and from 7 to end of the line
     - `echo -e "A,B,C\n1.22,1.2,3\n5,6,7\n9.99999,0,0" | cut -d "," -f 1` get the first column of a CSV (`-d` specifies the delimiter among field, `-f n` specifies to pick the n-th field from each line)
 - `find` is used to find files in specified directories that meet certain conditions. For example: `find . -type d -name "*lib*"` find all directories (not files) starting from the current one (`.`) whose name contain lib.
 - `locate` is less powerful than `find` but much faster since it relies on a database that is updated on a daily base or manually using the command `updatedb`. For example: `locate -i foo` finds all files or directories whose name contains `foo` ignoring case.
 - `tr` stands for **translate**. It supports a range of transformations including uppercase to lowercase, squeezing repeating characters, deleting specific characters, and basic find and replace. For instance:
     - `echo "Welcome to apsc labs" | tr [a-z] [A-Z]` converts all characters to upper case.  
     - `echo -e "A;B;c\n1,2;1,4;1,8" | tr "," "." | tr ";" ","` translates a line of a CSV in italian format to a standard format.
     - `echo "my ID is 73535" | tr -d [:digit:]` deletes all the digits from the string

---