/*
--- Day 8: Two-Factor Authentication ---
You come across a door implementing what you can only assume is an implementation of two-factor authentication after a long game of requirements telephone.

To get past the door, you first swipe a keycard (no problem; there was one on a nearby desk). Then, it displays a code on a little screen, and you type that code on a keypad. Then, presumably, the door unlocks.

Unfortunately, the screen has been smashed. After a few minutes, you've taken everything apart and figured out how it works. Now you just have to work out what the screen would have displayed.

The magnetic strip on the card you swiped encodes a series of instructions for the screen; these instructions are your puzzle input. The screen is 50 pixels wide and 6 pixels tall, all of which start off, and is capable of three somewhat peculiar operations:

rect AxB turns on all of the pixels in a rectangle at the top-left of the screen which is A wide and B tall.
rotate row y=A by B shifts all of the pixels in row A (0 is the top row) right by B pixels. Pixels that would fall off the right end appear at the left end of the row.
rotate column x=A by B shifts all of the pixels in column A (0 is the left column) down by B pixels. Pixels that would fall off the bottom appear at the top of the column.
For example, here is a simple sequence on a smaller screen:

rect 3x2 creates a small rectangle in the top-left corner:

###....
###....
.......
rotate column x=1 by 1 rotates the second column down by one pixel:

#.#....
###....
.#.....
rotate row y=0 by 4 rotates the top row right by four pixels:

....#.#
###....
.#.....
rotate column x=1 by 1 again rotates the second column down by one pixel, causing the bottom pixel to wrap back to the top:

.#..#.#
#.#....
.#.....
As you can see, this display technology is extremely powerful, and will soon dominate the tiny-code-displaying-screen market. That's what the advertisement on the back of the display tries to convince you, anyway.

There seems to be an intermediate check of the voltage used by the display: after you swipe your card, if the screen did work, how many pixels should be lit?
*/

using Core

func day8A()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "8.txt")
    let lines    = assume File.readAllLines(dataPath)

    const W = 50
    const H = 6
    var screen, nscreen: [W, H] bool
    var re: Parser.RegExp

    foreach l in lines
    {
        nscreen = screen
        switch
        {
        case assume re.match(#"rect (\d+)x(\d+)"#, l):
            for x in assume StrConv.toS32(re[1]) do
                for y in assume StrConv.toS32(re[2]) do
                    screen[x, y] = true

        case assume re.match(#"rotate column x=(\d+) by (\d+)"#, l):
            let x = assume StrConv.toS32(re[1])
            for assume StrConv.toS32(re[2])
            {
                for i in H do
                    screen[x, (i + 1) % H] = nscreen[x, i]
                nscreen = screen
            }

        case assume re.match(#"rotate row y=(\d+) by (\d+)"#, l):
            let y = assume StrConv.toS32(re[1])
            for assume StrConv.toS32(re[2])
            {
                for i in W do
                    screen[(i + 1) % W, y] = nscreen[i, y]
                nscreen = screen
            }
        }
    }

    var total = 0
    for y in H
    {
        for x in W
        {
            total += screen[x, y] ? 1 : 0
            //@print(screen[x, y] ? "X" : ".")
        }

        //@print("\n")
    }

    @assert(total == 116)
}
