/*
--- Part Two ---
It turns out that this circuit is very timing-sensitive; you actually need to minimize the signal delay.

To: this, calculate the number of steps each wire takes to reach each intersection; choose the intersection where the sum of both wires' steps is lowest. If a wire visits a position on the grid multiple times, use the steps value from the first time it visits that position when calculating the total value of a specific intersection.

The number of steps a wire takes is the total number of grid squares the wire has entered to get to that location, including the intersection being considered. Again consider the example from above:

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
In the above example, the intersection closest to the central port is reached after 8+5+5+2 = 20 steps by the first wire and 7+6+4+3 = 20 steps by the second wire for a total of 20+20 = 40 steps.

However, the top-right intersection is better: the first wire takes only 8+5+2 = 15 and the second wire takes only 7+6+2 = 15, a total of 15+15 = 30 steps.

Here are the best steps for the extra examples from above:

R75,D30,R83,U83,L12,D49,R71,U7,L72
U62,R66,U55,R34,D71,R55,D58,R83 = 610 steps
R98,U47,R26,D63,R33,U87,L62,D20,R33,U53,R51
U98,R91,D20,R16,D67,R40,U7,R15,U6,R7 = 410 steps
What is the fewest combined steps the wires must take to reach an intersection?*/

using Core, Core.Math

func day3B()
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

    var best = Swag.F32.Max
    var di   = 0'f32
    for i in t1.count - 1
    {
        var dj = 0'f32
        for j in t0.count - 1
        {
            let (yes, pos) = Geometry.segSegIntersect(t1[i], t1[i + 1], t0[j], t0[j + 1])
            if yes and !pos.isZero()
            {
                let d0 = pos - t0[j]
                let d1 = pos - t1[i]
                best = Math.min(best, di + dj + d0.length() + d1.length())
            }

            let d0 = t0[j + 1] - t0[j]
            dj += d0.length()
        }

        let d1 = t1[i + 1] - t1[i]
        di += d1.length()
    }

    //Console.print("total: ", best)
    @assert(best == 11222)
}
