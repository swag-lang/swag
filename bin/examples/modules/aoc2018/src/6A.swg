/*
--- Day 6: Chronal Coordinates ---
The device on your wrist beeps several times, and once again you feel like you're falling.

"Situation critical," the device announces. "Destination indeterminate. Chronal interference detected. Please specify new target coordinates."

The device then produces a list of coordinates (your puzzle input). Are they places it thinks are safe or dangerous? It recommends you check manual page 729. The Elves did not give you a manual.

If they're dangerous, maybe you can minimize the danger by finding the coordinate that gives the largest distance from the other points.

Using only the Manhattan distance, determine the area around each coordinate by counting the number of integer X,Y locations that are closest to that coordinate (and aren't tied in distance to any other coordinate).

Your goal is to find the size of the largest area that isn't infinite. For example, consider the following list of coordinates:

1, 1
1, 6
8, 3
3, 4
5, 5
8, 9
If we name these coordinates A through F, we can draw them on a grid, putting 0,0 at the top left:

..........
.A........
..........
........C.
...D......
.....E....
.B........
..........
..........
........F.
This view is partial - the actual grid extends infinitely in all directions. Using the Manhattan distance, each location's closest coordinate can be determined, shown here in lowercase:

aaaaa.cccc
aAaaa.cccc
aaaddecccc
aadddeccCc
..dDdeeccc
bb.deEeecc
bBb.eeee..
bbb.eeefff
bbb.eeffff
bbb.ffffFf
Locations shown as . are equally far from two or more coordinates, and so they don't count as being closest to any.

In this example, the areas of coordinates A, B, C, and F are infinite - while not shown here, their areas extend forever outside the visible grid. However, the areas of coordinates D and E are finite: D is closest to 9 locations, and E is closest to 17 (both including the coordinate's location itself). Therefore, in this example, the size of the largest area is 17.

What is the size of the largest area that isn't infinite?
*/

using Core

func day6A()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "6.txt")
    let lines    = assume File.readAllLines(dataPath)

    var coords: Array'{ x, y: s32 }
    foreach l in lines
    {
        let split = Tokenize.split(l, ',''u8)
        let vx    = assume StrConv.toS32(Latin1.trim(split[0]))
        let vy    = assume StrConv.toS32(Latin1.trim(split[1]))
        coords.add({vx, vy})
    }

    const W = 300

    var arr:     Array'{ d, who: s32 }
    var count:   Array's32
    var borders: Array's32

    arr.resize(W * W)
    count.resize(coords.count)
    borders.resize(coords.count)

    foreach c, who in coords
    {
        for i in W * W
        {
            let x = i % W
            let y = i / W
            let d = Math.abs(c.x - x) + Math.abs(c.y - y)

            var r = #ref arr.ref2(x, y, W)
            if d < r.d or !r.who
            {
                let border = cast(s32) (!x or !y or x == W - 1 or y == W - 1)

                if r.who
                {
                    count[r.who - 1] -= 1
                    borders[r.who - 1] -= border
                }

                r = {d, cast(s32) who + 1}

                count[who] += 1
                borders[who] += border
            }
        }
    }

    var total = 0
    foreach v in count do
        if !borders[@index] do
            total = Math.max(v, total)

    //Console.print("total: ", total)
    @assert(total == 3882)
}
