/*
--- Day 1: No Time for a Taxicab ---
Santa's sleigh uses a very high-precision clock to guide its movements, and the clock's oscillator is regulated by stars. Unfortunately, the stars have been stolen... by the Easter Bunny. To save Christmas, Santa needs you to retrieve all fifty stars by December 25th.

Collect stars by solving puzzles. Two puzzles will be made available on each day in the Advent calendar; the second puzzle is unlocked when you complete the first. Each puzzle grants one star. Good luck!

You're airdropped near Easter Bunny Headquarters in a city somewhere. "Near", unfortunately, is as close as you can get - the instructions on the Easter Bunny Recruiting Document the Elves intercepted start here, and nobody had time to work them out further.

The Document indicates that you should start at the given coordinates (where you just landed) and face North. Then, follow the provided sequence: either turn left (L) or right (R) 90 degrees, then walk forward the given number of blocks, ending at a new intersection.

There's no time to follow such ridiculous instructions on foot, though, so you take a moment and work out the destination. Given that you can only walk on the street grid of the city, how far is the shortest path to the destination?

For example:

Following R2, L3 leaves you 2 blocks East and 3 blocks North, or 5 blocks away.
R2, R2, R2 leaves you 2 blocks due South of your starting position, which is 2 blocks away.
R5, L5, R5, R3 leaves you 12 blocks away.
How many blocks away is Easter Bunny HQ?
*/

using Core

func day2B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "2.txt")
    let lines = assume File.readAllLines(dataPath)

    const pad: [7,7] s32 =
        [
            [0, 0, 0,  0,  0,  0, 0],
            [0, 0, 0,  1,  0,  0, 0],
            [0, 0, 2,  3,  4,  0, 0],
            [0, 5, 6,  7,  8,  9, 0],
            [0, 0, 10, 11, 12, 0, 0],
            [0, 0, 0,  13, 0,  0, 0],
            [0, 0, 0,  0,  0,  0, 0],
        ]

    var x = 1, y = 3, cde = 0
    visit l: lines
    {
        visit,bytes c: l
        {
            switch c
            {
            case `U`: if pad[y-1,x] do y -= 1
            case `D`: if pad[y+1,x] do y += 1
            case `L`: if pad[y,x-1] do x -= 1
            case `R`: if pad[y,x+1] do x += 1
            }
        }

        cde <<= 4
        cde |= pad[y,x]
    }

    //Console.printf("%{X} %", cde, cde)
    @assert(cde == 841940)
}

