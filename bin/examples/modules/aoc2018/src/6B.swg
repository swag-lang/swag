/*
--- Part Two ---
On the other hand, if the coordinates are safe, maybe the best you can: is try to find a region near as many coordinates as possible.

For example, suppose you want the sum of the Manhattan distance to all of the coordinates to be less than 32. For each location, add up the distances to all of the given coordinates; if the total of those distances is less than 32, that location is within the desired region. Using the same coordinates as above, the resulting region looks like this:

..........
.A........
..........
...###..C.
..#D###...
..###E#...
.B.###....
..........
..........
........F.
In particular, consider the highlighted location 4,3 located at the top middle of the region. Its calculation is as follows, where abs() is the absolute value function:

Distance to coordinate A: abs(4-1) + abs(3-1) =  5
Distance to coordinate B: abs(4-1) + abs(3-6) =  6
Distance to coordinate C: abs(4-8) + abs(3-3) =  4
Distance to coordinate D: abs(4-3) + abs(3-4) =  2
Distance to coordinate E: abs(4-5) + abs(3-5) =  3
Distance to coordinate F: abs(4-8) + abs(3-9) = 10
Total distance: 5 + 6 + 4 + 2 + 3 + 10 = 30
Because the total distance to all coordinates (30) is less than 32, the location is within the region.

This region, which also includes coordinates D and E, has a total size of 16.

Your actual region will need to be much larger than this example, though, instead including all locations with a total distance of less than 10000.

What is the size of the region containing all locations which have a total distance to all given coordinates of less than 10000?
*/

using Core

//#[Swag.PrintBc]
func day6B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "6.txt")
    var lines    = assume File.readAllLines(dataPath)

    const W = 700

    var arr: Array's32
    arr.resize(W * W)

    foreach l in lines
    {
        let split = Tokenize.split(l, ',''u8)
        let cx    = assume StrConv.toS32(Latin1.trim(split[0])) + W / 2
        let cy    = assume StrConv.toS32(Latin1.trim(split[1])) + W / 2

        for i in W * W
        {
            let x = i % W
            let y = i / W
            let d = Math.abs(cx - x) + Math.abs(cy - y)
            arr[i] += d
        }
    }

    var total = 0
    foreach v in arr:
        total += cast(s32) (v < 10000)

    //Console.print("total: ", total)
    @assert(total == 43852)
}
