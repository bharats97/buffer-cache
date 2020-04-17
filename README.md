# Buffer Cache

This is where we'll put all of our work.

### Some conventions that we'll follow

- Say no to Windows! Linux (64 bit) only.
- Always make sure to trim trailing white spaces, and say no to ```CRLF``` line endings (ensure ```LF``` line endings).
- All files must end with an ```LF``` character before an ```EOF```.
- No hard tabs please, a soft tab with a tab width of 4 spaces is what is expected here.

### How to start?

It will be preferred to run the following ```BASH``` commands before starting anything:

```bash
sudo apt update
sudo apt install -y git g++ gdb libboost-all-dev dos2unix
```

Then do:

```bash
git clone 'https://github.com/bharats97/buffer-cache.git'
cd buffer-cache/
touch tasks.txt
mkdir work_in_progress/
```

### How will we compile code?

```bash
g++ -no-pie -fno-pie -std=gnu++17 -O3 -pthread -Wall -Wextra -Wpedantic -D LOCAL -g -o ExecutableName FileName
```
where we'll try to keep the _ExecutableName_ and _FileName_ (before extension) same, and try to keep it logical (or follow some good convention or structure).

### Points to Remember

- ```tasks.txt``` and ```work_in_progress/``` are mentioned in ```.gitignore``` and hence won't be staged for commits. So any notes you want to make should be in the file ```tasks.txt``` and any code/work that you're doing and is not finalized should reside in the directory ```work_in_progress/```.
- Do not place any executable into the repository. If you want to compile and test code or generate executables, it should be done in ```work_in_progress/``` directory. Once finalized, the code should be moved to respective directory in the repository.

Rest anything useful will be updated here.
