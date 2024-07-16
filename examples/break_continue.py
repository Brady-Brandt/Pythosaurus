def print_sep():
    print("-------------------")


x = 0
while x < 1000:
    if x == 50:
        break 
    if x % 2 == 0:
        x += 1
        continue
    print(x)
    x += 1

print_sep()

i = 1
while i < 10:
    j = 1
    while j < 10:
        if j == 5:
            break
        print(i * j)
        j += 1
    i += 1

print_sep()

j = 0
while j < 10:
    k = 0
    print(j)
    while k < 5:
        if k == 3:
            k += 1
            continue
        print(k)
        k += 1
    if j == 5:
        break
    j += 1
