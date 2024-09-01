/*
--- Part Two ---
Unfortunately, considering only horizontal and vertical lines doesn't give you the full picture; you need to also consider diagonal lines.

Because of the limits of the hydrothermal vent mapping system, the lines in your list will only ever be horizontal, vertical, or a diagonal line at exactly 45 degrees. In other words:

An entry like 1,1 -> 3,3 covers points 1,1, 2,2, and 3,3.
An entry like 9,7 -> 7,9 covers points 9,7, 8,8, and 7,9.
Considering all lines from the above example would now produce the following diagram:

1.1....11.
.111...2..
..2.1.111.
...1.2.2..
.112313211
...1.2....
..1...1...
.1.....1..
1.......1.
222111....
You still need to determine the number of points where at least two lines overlap. In the above example, this is still anywhere in the diagram with a 2 or larger - now a total of 12 points.

Consider all of the lines. At how many points: at least two lines overlap?
*/

using Core

func day5B()
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
            let x0 = StrConv.toS32(r.getCapture(1))
            let y0 = StrConv.toS32(r.getCapture(2))
            let x1 = StrConv.toS32(r.getCapture(3))
            let y1 = StrConv.toS32(r.getCapture(4))
        }

        for i in Math.max(Math.abs(x1 - x0), Math.abs(y1 - y0)) + 1
        {
            let x: u64 = x0 + i * Math.sign(x1 - x0)
            let y: u64 = y0 + i * Math.sign(y1 - y0)
            let c  = #ref m.ref2(x, y, W)
            total += (c == 1 ? 1 : 0)
            c += 1
        }
    }

    //Console.print("total: ", total)
    @assert(total == 17604)
}
