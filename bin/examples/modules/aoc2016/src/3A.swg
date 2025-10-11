/*
--- Day 3: Squares With Three Sides ---
Now that you can think clearly, you move deeper into the labyrinth of hallways and office furniture that makes up this part of Easter Bunny HQ. This must be a graphic design department; the walls are covered in specifications for triangles.

Or are they?

The design document gives the side lengths of each triangle it describes, but... 5 10 25? Some of these aren't triangles. You can't help but mark the impossible ones.

In a valid triangle, the sum of any two sides must be larger than the remaining side. For example, the "triangle" given above is impossible, because 5 + 10 is not larger than 25.

In your puzzle input, how many of the listed triangles are possible?
*/

using Core

func day3A()
{
    assume
    {
        let dataPath = Path.combine(Path.getDirectoryName(#file), "3.txt")
        let lines    = File.readAllLines(dataPath)

        var total = 0
        foreach l in lines
        {
            let split = Tokenize.split(l, ' ''u8)
            let v     = [StrConv.toS32(split[0]), StrConv.toS32(split[1]), StrConv.toS32(split[2])]

            var ok = 0
            for i in 3 do
                ok += cast(s32) (v[i] + v[(i + 1) % 3] > v[(i + 2) % 3])
            total += cast(s32) (ok == 3)
        }

        //Console.print("total: ", total)
        @assert(total == 869)
    }
}
