/*
--- Part Two ---
While The Historians begin working around the guard's patrol route, you borrow their fancy device and step outside the lab. From the safety of a supply closet, you time travel through the last few months and record the nightly status of the lab's guard post on the walls of the closet.

Returning after what seems like only a few seconds to The Historians, they explain that the guard's patrol area is simply too large for them to safely search the lab without getting caught.

Fortunately, they are pretty sure that adding a single new obstruction won't cause a time paradox. They'd like to place the new obstruction in such a way that the guard will get stuck in a loop, making the rest of the lab safe to search.

To have the lowest chance of creating a time paradox, The Historians would like to know all of the possible positions for such an obstruction. The new obstruction can't be placed at the guard's starting position - the guard is there right now and would notice.

In the above example, there are only 6 different positions where a new obstruction would cause the guard to get stuck in a loop. The diagrams of these six situations use O to mark the new obstruction, | to show a position where the guard moves up/down, - to show a position where the guard moves left/right, and + to show a position where the guard moves both up/down and left/right.

Option one, put a printing press next to the guard's starting position:

....#.....
....+---+#
....|...|.
..#.|...|.
....|..#|.
....|...|.
.#.O^---+.
........#.
#.........
......#...
Option two, put a stack of failed suit prototypes in the bottom right quadrant of the mapped area:


....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
......O.#.
#.........
......#...
Option three, put a crate of chimney-squeeze prototype fabric next to the standing desk in the bottom right quadrant:

....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
.+----+O#.
#+----+...
......#...
Option four, put an alchemical retroencabulator near the bottom left corner:

....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
..|...|.#.
#O+---+...
......#...
Option five, put the alchemical retroencabulator a bit to the right instead:

....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
....|.|.#.
#..O+-+...
......#...
Option six, put a tank of sovereign glue right next to the tank of universal solvent:

....#.....
....+---+#
....|...|.
..#.|...|.
..+-+-+#|.
..|.|.|.|.
.#+-^-+-+.
.+----++#.
#+----++..
......#O..
It doesn't really matter what you choose to use as an obstacle so long as you and The Historians can put it into position without the guard noticing. The important thing is having enough options that you can find one that minimizes time paradoxes, and in this example, there are 6 different positions you could choose.

You need to get the guard stuck in a loop by adding a single new obstruction. How many different positions could you choose for this obstruction?
*/

using Core

private var w, h: s32

func day6B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "6.txt")
    let lines    = assume File.readAllLines(dataPath)

    var arr0: Array'u8
    var sx, sy: s32
    w = cast() lines[0].length
    h = cast() lines.count

    foreach l in lines
    {
        if !sx
        {
            let f = l.indexOf("^")
            if f != Swag.U64.Max do
                sx = cast() f
            else do
                sy += 1
        }

        arr0.add(l.toSlice())
    }

    func test(arr: *Array'u8, sx0, sy0: s32, first: bool)->bool
    {
        var sx   = sx0
        var sy   = sy0
        var dx = 0, dy = -1
        var what = 'D''u8
        for
        {
            if sx + dx < 0 or sx + dx >= w or sy + dy < 0 or sy + dy >= h
            {
                arr.ref2(sx, sy, w) = what
                return false
            }

            if arr.ref2(sx + dx, sy + dy, w) == '#'
            {
                if dx == 0 and dy == -1
                {
                    dx   = 1
                    dy   = 0
                    what = 'A'
                }
                elif dx == 1 and dy == 0
                {
                    dx   = 0
                    dy   = 1
                    what = 'B'
                }
                elif dx == 0 and dy == 1
                {
                    dx   = -1
                    dy   = 0
                    what = 'C'
                }
                else
                {
                    dx   = 0
                    dy   = -1
                    what = 'D'
                }

                continue
            }

            let r = #ref arr.ref2(sx, sy, w)

            if first do
                what = 'O'
            elif r == what do
                return true
            r = what

            sx += dx
            sy += dy
        }

        return true
    }

    //Time.Stopwatch.scopeMeasure()

    discard test(&arr0, sx, sy, true)

    var total = 0
    foreach v in arr0
    {
        if v != 'O' do
            continue

        var arr1 = arr0
        arr1[@index] = '#'

        if test(&arr1, sx, sy, false) do
            total += 1
    }

    @assert(total == 1836)
}
