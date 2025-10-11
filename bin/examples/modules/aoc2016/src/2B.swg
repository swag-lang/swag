/*
--- Part Two ---
You finally arrive at the bathroom (it's a several minute walk from the lobby so visitors can behold the many fancy conference rooms and water coolers on this floor) and go to punch in the code. Much to your bladder's dismay, the keypad is not at all like you imagined it. Instead, you are confronted with the result of hundreds of man-hours of bathroom-keypad-design meetings:

    1
  2 3 4
5 6 7 8 9
  A B C
    D
You still start at "5" and stop when you're at an edge, but given the same instructions as above, the outcome is very different:

You start at "5" and don't move at all (up and left are both edges), ending at 5.
Continuing from "5", you move right twice and down three times (through "6", "7", "B", "D", "D"), ending at D.
Then, from "D", you move five more times (through "D", "B", "C", "C", "B"), ending at B.
Finally, after five more moves, you end at 3.
So, given the actual keypad layout, the code would be 5DB3.

Using the same instructions in your puzzle input, what is the correct bathroom code?
*/

using Core

func day2B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "2.txt")
    let lines    = assume File.readAllLines(dataPath)

    const pad: [7, 7] s32 = [
        [0, 0, 0, 0, 0, 0, 0],
        [0, 0, 0, 1, 0, 0, 0],
        [0, 0, 2, 3, 4, 0, 0],
        [0, 5, 6, 7, 8, 9, 0],
        [0, 0, 10, 11, 12, 0, 0],
        [0, 0, 0, 13, 0, 0, 0],
        [0, 0, 0, 0, 0, 0, 0]]
    var x      = 1
    var y      = 3

    var cde = 0
    foreach l in lines
    {
        foreach #Bytes c in l
        {
            switch c
            {
            case 'U':
                if pad[y - 1, x] do
                    y -= 1
            case 'D':
                if pad[y + 1, x] do
                    y += 1
            case 'L':
                if pad[y, x - 1] do
                    x -= 1
            case 'R':
                if pad[y, x + 1] do
                    x += 1
            }
        }

        cde <<= 4
        cde |= pad[y, x]
    }

    //Console.printf("%{X} %", cde, cde)
    @assert(cde == 841940)
}
