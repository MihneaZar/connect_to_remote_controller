from random import randint

def transmit_char(c, transmission_rate):
    if randint(0, 100) < transmission_rate or c == '\n':
        return c
    else:
        return chr(ord(c) + randint(0, 255))


def main():
    transmission_rate = int(input("Transmission efficacy, as a percentage ((100 - efficacy) is the likelihood a character is mistransmitted):\n"))
    max_space = int(input("\nSpace between two errors:\n"))

    # perhaps one of the worst one-liners ive ever written
    # print(''.join(map(lambda c: transmit_char(c, transmission_rate), list(open("transmission.txt").read()))))

    text = list(open("transmission.txt").read())
    errors = 0

    # print(round(((100 - transmission_rate + 1) / 100) * len(text)))
    # for i in range(0, round(((100 - transmission_rate + 1) / 100) * len(text))):
    #     randidx = randint(0, len(text) - 1)
    #     if text[randidx] != '\n':
    #         if transmission_rate < randint(0, 100):
    #             text[randidx] = chr(ord(text[randidx]) + randint(0, 255))
    #             errors = errors + 1
    #     else: 
    #         i -= 1
    # print(errors)

    space_errors = 0

    for i in range(0, len(text)):
        
        if space_errors != 0:
            space_errors -= 1

        if transmission_rate < randint(0, 101) and space_errors == 0 and text[i] != '\n':
            text[i] = chr(ord(text[i]) + randint(0, 255))
            errors += 1
            space_errors = randint(max_space / 2, max_space)

    print(errors)
    print(''.join(text))



# main function
if __name__ == "__main__":
    main()



