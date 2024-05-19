
def trap():
    """
    Deceptive trap function.
    Given fitness of each chromosome and schemata,
    Compute the fitness of the schemata.

    Example:
    f(0**) = average(f(000), f(001), f(010), f(011)) = 19
    """
    
    fitness = {"000": 28,
               "001": 26,
               "010": 22,
               "011": 0,
               "100": 14,
               "101": 0,
               "110": 0,
               "111": 30}
    
    schema = ["0**", "*0*", "**0", "1**", "*1*", "**1",
                "00*", "0*0", "*00", "11*", "1*1", "*11",
                "01*", "0*1", "*01", "10*", "1*0", "*10"]
    
    for s in schema:
        f = 0
        count = 0
        for i in range(2):
            for j in range(2):
                for k in range(2):
                    if s[0] == "*":
                        a = i
                    else:
                        a = int(s[0])
                    if s[1] == "*":
                        b = j
                    else:
                        b = int(s[1])
                    if s[2] == "*":
                        c = k
                    else:
                        c = int(s[2])
                    f += fitness[str(a) + str(b) + str(c)]
                    count += 1
        print(f"{s}: {f/count}")
    
    


if __name__ == '__main__':
    trap()