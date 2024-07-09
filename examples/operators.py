# Addition
assert 2 + 3 == 5, "Add"

# Subtraction
assert 5 - 3 == 2

# Multiplication
assert 2 * 3 == 6

# Division
#assert 10 / 2 == 5

# Floor Division
assert 10 // 3 == 3

# Modulus
assert 10 % 3 == 1

# Exponentiation
assert 2 ** 3 == 8

# Equal to
assert 5 == 5

# Not equal to
assert 5 != 3

# Greater than
assert 5 > 3

# Less than
assert 3 < 5

# Greater than or equal to
assert 5 >= 5

# Less than or equal to
assert 3 <= 5

# Logical AND
assert True and True
assert not (False and True)

# Logical OR
assert True or False
assert not (False or False)

# Logical NOT
assert not False
assert not (not True)


str1 = "Hello, "
str2 = "world!"
result = str1 + str2
assert result == "Hello, world!", "String concat Failure"

str1 = "Python"
result = str1 * 3
assert result == "PythonPythonPython", "String Multiplication Failure"

