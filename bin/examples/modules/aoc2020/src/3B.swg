/*
--- Part Two ---
Time to check the rest of the slopes - you need to minimize the probability of a sudden arboreal stop, after all.

Determine the number of trees you would encounter if, for each of the following slopes, you start at the top-left corner and traverse the map all the way to the bottom:

Right 1, down 1.
Right 3, down 1. (This is the slope you already checked.)
Right 5, down 1.
Right 7, down 1.
Right 1, down 2.
In the above example, these slopes would find 2, 7, 3, 4, and 2 tree(s) respectively; multiplied together, these produce the answer 336.

What: you get if you multiply together the number of trees encountered on each of the listed slopes?
*/

using Core

func day3B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "3.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total: u64 = 1
    foreach s, slope in [{1, 1}, {3, 1}, {5, 1}, {7, 1}, {1, 2}]
    {
        var x, y, count: u32
        for
        {
            y += s.item1
            if y >= lines.count do
                break
            x = cast(u32) ((x + s.item0) % lines[0].length)
            count += lines[y][x] == '#' ? 1 : 0
        }

        total *= count
    }

    //Console.print("total: ", total)
    @assert(total == 3584591857)
}
