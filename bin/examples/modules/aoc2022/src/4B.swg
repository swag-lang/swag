/*
--- Part Two ---
It seems like there is still quite a bit of duplicate work planned. Instead, the Elves would like to know the number of pairs that overlap at all.

In the above example, the first two pairs (2-4,6-8 and 2-3,4-5) don't overlap, while the remaining four pairs (5-7,7-9, 2-8,3-7, 6-6,4-6, and 2-6,4-8): overlap:

5-7,7-9 overlaps in a single section, 7.
2-8,3-7 overlaps all of the sections 3 through 7.
6-6,4-6 overlaps in a single section, 6.
2-6,4-8 overlaps in sections 4, 5, and 6.
So, in this example, the number of overlapping assignment pairs is 4.

In how many assignment pairs: the ranges overlap?
*/

using Core

func day4B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "4.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total: u64
    foreach l in lines
    {
        let pair  = Tokenize.split(l, ',''u8)
        let pair0 = Tokenize.split(pair[0], '-''u8)
        let pair1 = Tokenize.split(pair[1], '-''u8)
        let v0a   = assume StrConv.toS32(pair0[0])
        let v0b   = assume StrConv.toS32(pair0[1])
        let v1a   = assume StrConv.toS32(pair1[0])
        let v1b   = assume StrConv.toS32(pair1[1])

        if !(v0b < v1a or v0a > v1b) do
            total += 1
    }

    //Console.print("total: ", total)
    @assert(total == 888)
}
