/*
--- Part Two ---
You quickly reach the farthest point of the for, but the animal never emerges. Maybe its nest is within the area enclosed by the for?

To determine whether it's even worth taking the time to search for such a nest, you should calculate how many tiles are contained within the for. For example:

...........
.S-------7.
.|F-----7|.
.||.....||.
.||.....||.
.|L-7.F-J|.
.|..|.|..|.
.L--J.L--J.
...........
The above for encloses merely four tiles - the two pairs of . in the southwest and southeast (marked I below). The middle . tiles (marked O below) are not in the for. Here is the same for again with those regions marked:

...........
.S-------7.
.|F-----7|.
.||OOOOO||.
.||OOOOO||.
.|L-7OF-J|.
.|II|O|II|.
.L--JOL--J.
.....O.....
In fact, there doesn't even need to be a full tile path to the outside for tiles to count as outside the for - squeezing between pipes is also allowed! Here, I is still within the for and O is still outside the for:

..........
.S------7.
.|F----7|.
.||OOOO||.
.||OOOO||.
.|L-7F-J|.
.|II||II|.
.L--JL--J.
..........
In both of the above examples, 4 tiles are enclosed by the for.

Here's a larger example:

.F----7F7F7F7F-7....
.|F--7||||||||FJ....
.||.FJ||||||||L7....
FJL7L7LJLJ||LJ.L-7..
L--J.L7...LJS7F-7L7.
....F-J..F7FJ|L7L7L7
....L7.F7||L7|.L7L7|
.....|FJLJ|FJ|F7|.LJ
....FJL-7.||.||||...
....L---J.LJ.LJLJ...
The above sketch has many random bits of ground, some of which are in the for (I) and some of which are outside it (O):

OF----7F7F7F7F-7OOOO
O|F--7||||||||FJOOOO
O||OFJ||||||||L7OOOO
FJL7L7LJLJ||LJIL-7OO
L--JOL7IIILJS7F-7L7O
OOOOF-JIIF7FJ|L7L7L7
OOOOL7IF7||L7|IL7L7|
OOOOO|FJLJ|FJ|F7|OLJ
OOOOFJL-7O||O||||OOO
OOOOL---JOLJOLJLJOOO
In this larger example, 8 tiles are enclosed by the for.

Any tile that isn't part of the main for can count as being enclosed by the for. Here's another example with many bits of junk pipe lying around that aren't connected to the main for at all:

FF7FSF7F7F7F7F7F---7
L|LJ||||||||||||F--J
FL-7LJLJ||||||LJL-77
F--JF--7||LJLJ7F7FJ-
L---JF-JLJ.||-FJLJJ7
|F|F-JF---7F7-L7L|7|
|FFJF7L7F-JF7|JL---7
7-L-JL7||F7|L7F-7F7|
L.L7LFJ|||||FJL7||LJ
L7JLJL-JLJLJL--JLJ.L
Here are just the tiles that are enclosed by the for marked with I:

FF7FSF7F7F7F7F7F---7
L|LJ||||||||||||F--J
FL-7LJLJ||||||LJL-77
F--JF--7||LJLJIF7FJ-
L---JF-JLJIIIIFJLJJ7
|F|F-JF---7IIIL7L|7|
|FFJF7L7F-JF7IIL---7
7-L-JL7||F7|L7F-7F7|
L.L7LFJ|||||FJL7||LJ
L7JLJL-JLJLJL--JLJ.L
In this last example, 10 tiles are enclosed by the for.

Figure out whether you have time to search for the nest by calculating the area within the for. How many tiles are enclosed by the for?
*/

using Core

#[Swag.EnumFlags]
private enum Dir
{
    Zero  = 0
    West
    East
    North
    South
    Edge
    Fill
    Mask  = 1 | 2 | 4 | 8
}

func day10B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "10.txt")
    let lines    = assume File.readAllLines(dataPath)

    var width, height: u64
    var maze: Array'Dir

    width  = lines[0].length + 2
    height = lines.count + 2
    maze.resize(width * height)

    #[Swag.Mixin]
    func mp(x0, y0: s64)->&Dir => maze.buffer + ((x0 + 1) + (y0 + 1) * width)

    var startX, startY: s64
    foreach l, line in lines
    {
        foreach #Bytes c, col in l
        {
            let one = #ref mp(cast(s64) col, cast(s64) line)
            switch c
            {
            case '-':
                one = .West | .East
            case '|':
                one = .North | .South
            case 'L':
                one = .North | .East
            case 'J':
                one = .North | .West
            case '7':
                one = .South | .West
            case 'F':
                one = .South | .East
            case 'S':
                startX = col
                startY = line
            }
        }
    }

    // Determin the shape of the start 'S'
    let start = #ref mp(startX, startY)
    if mp(startX - 1, startY) & .East do
        start |= .West
    if mp(startX + 1, startY) & .West do
        start |= .East
    if mp(startX, startY - 1) & .South do
        start |= .North
    if mp(startX, startY + 1) & .North do
        start |= .South
    var visitX = startX

    // Visit the maze
    var visitY = startY
    for
    {
        let one = #ref mp(visitX, visitY)
        one |= .Edge

        switch
        {
        case one & .East and !(mp(visitX + 1, visitY) & .Edge):
            visitX += 1
            continue
        case one & .West and !(mp(visitX - 1, visitY) & .Edge):
            visitX -= 1
            continue
        case one & .North and !(mp(visitX, visitY - 1) & .Edge):
            visitY -= 1
            continue
        case one & .South and !(mp(visitX, visitY + 1) & .Edge):
            visitY += 1
            continue
        }

        break
    }

    var total: u64
    foreach l, line in lines
    {
        foreach #Bytes c, col in l
        {
            let one = mp(cast(s64) col, cast(s64) line)
            if one & .Edge do
                continue

            var inside = false
            for x in col
            {
                let one1 = mp(cast(s64) x, cast(s64) line)
                if (one1 & .Edge and one1 & .North) do
                    inside = !inside
            }

            total += inside ? 1 : 0
        }
    }

    //Console.print("total: ", total, " ")
    @assert(total == 287)
}
