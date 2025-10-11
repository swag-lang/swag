/*
--- Part Two ---
Amidst the chaos, you notice that exactly one claim doesn't overlap by even a single square inch of fabric with any other claim. If you can somehow draw attention to it, maybe the Elves will be able to make Santa's suit after all!

For example, in the claims above, only claim 3 is intact after all claims are made.

What is the ID of the only claim that doesn't overlap?
*/

using Core

func day3B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "3.txt")
    let lines    = assume File.readAllLines(dataPath)

    var r = assume Parser.RegExp.create(#"#(\d+) @ (\d+),(\d+): (\d+)x(\d+)"#)

    var ids:  Array's32
    var here: HashSet's32
    ids.resize(1000 * 1000)
    here.reserve(1000)
    foreach l in lines
    {
        discard r.match(l)
        let id     = assume StrConv.toS32(r[1])
        let startX = assume StrConv.toS32(r[2])
        let startY = assume StrConv.toS32(r[3])
        let width  = assume StrConv.toS32(r[4])
        let height = assume StrConv.toS32(r[5])

        var alone = true
        for x in startX until startX + width
        {
            for y in startY until startY + height
            {
                let re = #ref ids.ref2(x, y, 1000)
                if re
                {
                    alone = false
                    here.remove(re)
                }
                re = id
            }
        }
        if alone do
            here.add(id)
    }

    @assert(here.count == 1)
    @assert(here.contains(251))
}
