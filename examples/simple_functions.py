def helloworld():
    return "Hello World"

def fact(n):
    if n == 1:
        return 1
    return n * fact(n - 1)

def fibonacci_recursive(n):
    if n <= 1:
        return n
    else:
        return fibonacci_recursive(n-1) + fibonacci_recursive(n-2)

assert fact(6) == 720, "Factorial is Wrong"
assert fibonacci_recursive(30) == 832040, "fib is wrong"
assert helloworld() == "Hello World"

