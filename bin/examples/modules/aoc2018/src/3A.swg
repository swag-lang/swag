/*
--- Day 3: No Matter How You Slice It ---
The Elves managed to locate the chimney-squeeze prototype fabric for Santa's suit (thanks to someone who helpfully wrote its box IDs on the wall of the warehouse in the middle of the night). Unfortunately, anomalies are still affecting them - nobody can even agree on how to cut the fabric.

The whole piece of fabric they're working on is a very large square - at least 1000 inches on each side.

Each Elf has made a claim about which area of fabric would be ideal for Santa's suit. All claims have an ID and consist of a single rectangle with edges parallel to the edges of the fabric. Each claim's rectangle is defined as follows:

The number of inches between the left edge of the fabric and the left edge of the rectangle.
The number of inches between the top edge of the fabric and the top edge of the rectangle.
The width of the rectangle in inches.
The height of the rectangle in inches.
A claim like #123 @ 3,2: 5x4 means that claim ID 123 specifies a rectangle 3 inches from the left edge, 2 inches from the top edge, 5 inches wide, and 4 inches tall. Visually, it claims the square inches of fabric represented by # (and ignores the square inches of fabric represented by .) in the diagram below:

...........
...........
...#####...
...#####...
...#####...
...#####...
...........
...........
...........
The problem is that many of the claims overlap, causing two or more claims to cover part of the same areas. For example, consider the following claims:

#1 @ 1,3: 4x4
#2 @ 3,1: 4x4
#3 @ 5,5: 2x2
Visually, these claim the following areas:

........
...2222.
...2222.
.11XX22.
.11XX22.
.111133.
.111133.
........
The four square inches marked with X are claimed by both 1 and 2. (Claim 3, while adjacent to the others, does not overlap either of them.)

If the Elves all proceed with their own plans, none of them will have enough fabric. How many square inches of fabric are within two or more claims?
*/

using Core

func day3A()
{
    day3A0()
    day3A1()
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day3A0()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "3.txt")
    let lines    = assume File.readAllLines(dataPath)

    var r      = assume Parser.RegExp.create(#"#\d+ @ (\d+),(\d+): (\d+)x(\d+)"#)
    var count: Array's32
    count.resize(1000 * 1000)
    foreach l in lines
    {
        discard r.match(l)
        let startX = assume StrConv.toS32(r[1])
        let startY = assume StrConv.toS32(r[2])
        let width  = assume StrConv.toS32(r[3])
        let height = assume StrConv.toS32(r[4])

        for x in startX until startX + width do
            for y in startY until startY + height do
                count.ref2(x, y, 1000) += 1
    }

    var total = 0
    foreach v in count do
        if v > 1 do
            total += 1

    //Console.print("total: ", total)
    @assert(total == 100261)
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day3A1()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "3.txt")
    let lines    = assume File.readAllLines(dataPath)

    var count: Array's32
    count.resize(1000 * 1000)
    foreach l in lines
    {
        let r = assume Parser.RegExp.grepAll("\\d+", l)
        let v = Slice.map(r[ to ], func(x) => assume StrConv.toS32(x))

        for x in v[1] until v[1] + v[3] do
            for y in v[2] until v[2] + v[4] do
                count.ref2(x, y, 1000) += 1
    }

    var total = 0
    foreach v in count do
        if v > 1 do
            total += 1

    //Console.print("total: ", total)
    @assert(total == 100261)
}
