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

Consider all of the lines. At how many points do at least two lines overlap?
*/

using Core

func day5B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#location.fileName), "5.txt")
    let lines = assume File.readAllLines(dataPath)

    var r: Parser.RegExp
    var m: Array's32
    var total = 0'u64

    const W = 1000'u64
    m.resize(W*W)

    visit l: lines
    {
        assume
        {
            discard r.match(#"(\d+),(\d+) -> (\d+),(\d+)"#, l)
            var x0 = StrConv.toS32(r.getCapture(1))
            var y0 = StrConv.toS32(r.getCapture(2))
            var x1 = StrConv.toS32(r.getCapture(3))
            var y1 = StrConv.toS32(r.getCapture(4))
        }

        if x0 != x1 and y0 != y1
        {
            loop i: Math.abs(x1-x0)+1
            {
                let x = x0 + i*Math.sign(x1-x0)
                let y = y0 + i*Math.sign(y1-y0)
                let c = ref (m.buffer + (x + y*W))
                total += (c == 1 ? 1 : 0)
                c += 1
            }
        }
        else
        {
            Core.orderMinMax(&x0, &x1)
            Core.orderMinMax(&y0, &y1)
            if x0 != x1
                loop x: x0 to x1
                {
                    let c = ref (m.buffer + (x + y0*W))
                    total += (c == 1 ? 1 : 0)
                    c += 1
                }
            else
                loop y: y0 to y1
                {
                    let c = ref (m.buffer + (x0 + y*W))
                    total += (c == 1 ? 1 : 0)
                    c += 1
                }
        }
    }

    //Console.print("total: ", total)
    @assert(total == 17604)
}

