/*
--- Part Two ---
Now that you've helpfully marked up their design documents, it occurs to you that triangles are specified in groups of three vertically. Each set of three numbers in a column specifies a triangle. Rows are unrelated.

For example, given the following specification, numbers with the same hundreds digit would be part of the same triangle:

101 301 501
102 302 502
103 303 503
201 401 601
202 402 602
203 403 603
In your puzzle input, and instead reading by columns, how many of the listed triangles are possible?
*/

using Core

func day3B()
{
    assume
    {
        let dataPath = Path.combine(Path.getDirectoryName(#file), "3.txt")
        let lines    = File.readAllLines(dataPath)

        var total = 0
        for var l = 0'u64; l < lines.count; l += 3
        {
            var split: [3] Array'string
            split[0] = Tokenize.split(lines[l], ' ''u8)
            split[1] = Tokenize.split(lines[l + 1], ' ''u8)
            split[2] = Tokenize.split(lines[l + 2], ' ''u8)

            for i in 3
            {
                let v  = [StrConv.toS32(split[0][i]), StrConv.toS32(split[1][i]), StrConv.toS32(split[2][i])]
                var ok = 0
                for j in 3 do
                    ok += cast(s32) (v[j] + v[(j + 1) % 3] > v[(j + 2) % 3])
                total += cast(s32) (ok == 3)
            }
        }

        //Console.print("total: ", 1544)
        @assert(total == 1544)
    }
}
