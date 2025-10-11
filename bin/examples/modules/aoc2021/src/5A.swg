/*
--- Day 5: Hydrothermal Venture ---
You come across a field of hydrothermal vents on the ocean floor! These vents constantly produce large, opaque clouds, so it would be best to avoid them if possible.

They tend to form in lines; the submarine helpfully produces a list of nearby lines of vents (your puzzle input) for you to review. For example:

0,9 -> 5,9
8,0 -> 0,8
9,4 -> 3,4
2,2 -> 2,1
7,0 -> 7,4
6,4 -> 2,0
0,9 -> 2,9
3,4 -> 1,4
0,0 -> 8,8
5,5 -> 8,2
Each line of vents is given as a line segment in the format x1,y1 -> x2,y2 where x1,y1 are the coordinates of one end the line segment and x2,y2 are the coordinates of the other end. These line segments include the points at both ends. In other words:

An entry like 1,1 -> 1,3 covers points 1,1, 1,2, and 1,3.
An entry like 9,7 -> 7,7 covers points 9,7, 8,7, and 7,7.
For now, only consider horizontal and vertical lines: lines where either x1 = x2 or y1 = y2.

So, the horizontal and vertical lines from the above list would produce the following diagram:

.......1..
..1....1..
..1....1..
.......1..
.112111211
..........
..........
..........
..........
222111....
In this diagram, the top left corner is 0,0 and the bottom right corner is 9,9. Each position is shown as the number of lines which cover that point or . if no line covers that point. The top-left pair of 1s, for example, comes from 2,2 -> 2,1; the very bottom row is formed by the overlapping lines 0,9 -> 5,9 and 0,9 -> 2,9.

To avoid the most dangerous areas, you need to determine the number of points where at least two lines overlap. In the above example, this is anywhere in the diagram with a 2 or larger - a total of 5 points.

Consider only horizontal and vertical lines. At how many points: at least two lines overlap?
*/

using Core

func day5A()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "5.txt")
    let lines    = assume File.readAllLines(dataPath)

    var r:    Parser.RegExp
    var m:    Array's32
    var total = 0'u64

    const W = 1000'u64
    m.resize(W * W)

    foreach l in lines
    {
        assume
        {
            discard r.match(#"(\d+),(\d+) -> (\d+),(\d+)"#, l)
            var x0 = StrConv.toS32(r.getCapture(1))
            var y0 = StrConv.toS32(r.getCapture(2))
            var x1 = StrConv.toS32(r.getCapture(3))
            var y1 = StrConv.toS32(r.getCapture(4))
        }

        if x0 != x1 and y0 != y1 do
            continue

        Core.orderMinMax(&x0, &x1)
        Core.orderMinMax(&y0, &y1)

        if x0 != x1 do
            for x in x0 to x1
            {
                let c = #ref (m.buffer + (x + y0 * W))
                total += (c == 1 ? 1 : 0)
                c += 1
            }
        elif y0 != y1 do
            for y in y0 to y1
            {
                let c = #ref (m.buffer + (x0 + y * W))
                total += (c == 1 ? 1 : 0)
                c += 1
            }
    }

    //Console.print("total: ", total)
    @assert(total == 5167)
}
