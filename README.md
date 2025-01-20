# Overview
Here’s an improved version of the description for your README:

Pythosaurus is a Python-like interpreter that I developed as a personal project to explore how programming languages are constructed. 
Python, being a complex and versatile language, provided an excellent foundation for this project.
However, rather than replicating every aspect of Python, Pythosaurus focuses on implementing a subset of the features 
I find most useful when writing Python code.

## Features 
This is a list of features that have already been added along with things that I am planning to add. This is list is not comprehensive and is subject to change. 

### Keywords
- [x] False
- [x] None
- [x] True
- [x] And
- [ ] As
- [x] Assert
- [x] Break
- [ ] Class
- [x] Continue
- [x] Def
- [x] Del
- [x] Elif
- [x] Else
- [ ] Except
- [ ] Finally
- [ ] For
- [ ] From
- [ ] Global
- [x] If
- [ ] Import
- [ ] In
- [ ] Is
- [ ] Lambda
- [ ] Nonlocal
- [x] Not
- [x] Or
- [x] Pass
- [ ] Raise
- [x] Return
- [ ] Try
- [x] While
- [ ] With
- [ ] Yield
### Types 
- [x] Strings
- [x] Integers (64 bit only)
- [x] Floats
- [x] Boolean
- [ ] List
- [ ] Dictionary

### Builtin Functions (No varargs or Kwargs yet)
- [x] abs
- [ ] all
- [ ] any
- [ ] ascii
- [x] bin
- [x] bool
- [ ] bytearray
- [ ] bytes
- [ ] callable
- [ ] chr
- [x] float
- [ ] format
- [ ] hash
- [x] hex
- [ ] id
- [x] input
- [x] int
- [x] len
- [ ] open
- [x] print
- [ ] round
- [ ] sorted
- [ ] type
### Misc
- [x] Math operations (Pretty much all math operations are implemented for bool, int, and floats)
- [x] String concat and multiply
- [ ] Reference counting

## Usage (Linux and MacOs)
Clone the repo and open the directory
```sh
git clone https://github.com/Brady-Brandt/Pythosaurus
cd Pythosaurus
```
Create the binary directory and run make
```sh
mkdir bin
make
```
Check out the examples or create your own scripts

## Final Thoughts 
As of right now Pythosaurus only includes a few basic Python features. The language I believe is technically turing complete though, 
so you can use it to develop programs. However, there are still a lot of issues with it and you are better of just using a regular Python interpretor.




