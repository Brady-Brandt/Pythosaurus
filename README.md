# Pythosaurus 

This is my attempt at building a Python like interpretor.I am not trying to fully recreate Python, but I want to try to implement 
as much of the main stuff as possible. Right now the interpetor is just a basic walk the syntax tree interpretor. I have a very basic
reference counted garbage collector implemented (still causes leaks). 

## Progress
I have only decided to implement things that I personally think are useful in Python. 
I am not trying to build a complete Python interpretor. I am just aimng to build something that you can write 
simple programs in. Below is a list of things that I have implemented or a planning on implementing. 
This list is not exhaustive. 

### Keywords
- [x] False
- [x] None
- [x] True
- [x] and
- [ ] as
- [x] assert
- [x] break
- [ ] class
- [x] continue
- [x] def
- [x] del
- [x] elif
- [x] else
- [ ] except
- [ ] finally
- [ ] for
- [ ] from
- [ ] global
- [x] if
- [ ] import
- [ ] in
- [ ] is
- [ ] lambda
- [ ] nonlocal
- [x] not
- [x] or
- [x] pass
- [ ] raise
- [x] return
- [ ] try
- [x] while
- [ ] with
- [ ] yield

### Data types 
- [x] int (Only 64 bit for now)
- [x] float
- [x] bool
- [x] str
- [ ] list
- [ ] tuple
- [ ] dict
- [ ] bytearray

### Builtin Functions 
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
