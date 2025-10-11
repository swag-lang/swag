/*
--- Day 3: Crossed Wires ---
The gravity assist was successful, and you're well on your way to the Venus refuelling station. During the rush back on Earth, the fuel management system wasn't completely installed, so that's next on the priority list.

Opening the front panel reveals a jumble of wires. Specifically, two wires are connected to a central port and extend outward on a grid. You trace the path each wire takes as it leaves the central port, one wire per line of text (your puzzle input).

The wires twist and turn, but the two wires occasionally cross paths. To fix the circuit, you need to find the intersection point closest to the central port. Because the wires are on a grid, use the Manhattan distance for this measurement. While the wires: technically cross right at the central port where they both start, this point does not count, nor does a wire count as crossing with itself.

For example, if the first wire's path is R8,U5,L5,D3, then starting from the central port (o), it goes right 8, up 5, left 5, and finally down 3:

...........
...........
...........
....+----+.
....|....|.
....|....|.
....|....|.
.........|.
.o-------+.
...........
Then, if the second wire's path is U7,R6,D4,L4, it goes up 7, right 6, down 4, and left 4:

...........
.+-----+...
.|.....|...
.|..+--X-+.
.|..|..|.|.
.|.-X--+.|.
.|..|....|.
.|.......|.
.o-------+.
...........
These wires cross at two locations (marked X), but the lower-left one is closer to the central port: its distance is 3 + 3 = 6.

Here are a few more examples:

R75,D30,R83,U83,L12,D49,R71,U7,L72
U62,R66,U55,R34,D71,R55,D58,R83 = distance 159
R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51
U98,R91,D20,R16,D67,R40,U7,R15,U6,R7 = distance 135
What is the Manhattan distance from the central port to the closest intersection?
*/

using Core, Core.Math

func day3A()
{
    day3A0()
    day3A1()
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
private const S = 20000
private var grid: [S, S] bool
private var total = Swag.S32.Max
func day3A0()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "3.txt")
    let lines    = assume File.readAllLines(dataPath)

    func trace(idx: s32, line: string)
    {
        var x, y = S / 2
        var dx, dy = 0
        foreach v in Tokenize.split(line, ',''u8)
        {
            switch v[0]
            {
            case 'U':
                dx = 0
                dy = -1
            case 'D':
                dx = 0
                dy = 1
            case 'R':
                dx = 1
                dy = 0
            case 'L':
                dx = -1
                dy = 0
            }

            let val = assume StrConv.toS32(v[1 to ])
            for val
            {
                x = x + dx
                y = y + dy
                if idx == 0 do
                    grid[x, y] = true
                if idx == 1 and grid[x, y]
                {
                    let d = Math.abs(x - S / 2) + Math.abs(y - S / 2)
                    total = Math.min(total, d)
                }
            }
        }
    }

    trace(0, lines[0])
    trace(1, lines[1])

    //Console.print("total: ", total)
    @assert(total == 303)
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day3A1()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "3.txt")
    let lines    = assume File.readAllLines(dataPath)

    func trace(line: string)->Array'Vector2
    {
        var result: retval
        var pos:    Vector2
        result.add(pos)
        foreach v in Tokenize.split(line, ',''u8)
        {
            let val = assume StrConv.toF32(v[1 to ])
            switch v[0]
            {
            case 'U':
                pos += {0, -val}
            case 'D':
                pos += {0, val}
            case 'R':
                pos += {-val, 0}
            case 'L':
                pos += {val, 0}
            }
            result.add(pos)
        }
        return result
    }

    let t0 = trace(lines[0])
    let t1 = trace(lines[1])

    var best = Swag.S32.Max
    for i in t1.count - 1
    {
        for j in t0.count - 1
        {
            let (yes, pos) = Geometry.segSegIntersect(t1[i], t1[i + 1], t0[j], t0[j + 1])
            if yes and !pos.isZero()
            {
                let d = Math.abs(pos.x) + Math.abs(pos.y)
                best = Math.min(best, cast(s32) d)
            }
        }
    }

    //Console.print("total: ", best)
    @assert(best == 303)
}
