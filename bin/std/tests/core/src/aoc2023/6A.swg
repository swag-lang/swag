// https://adventofcode.com/ 
// Year 2023

using Core

#test
{
    var dataPath: String = Path.getDirectoryName(#location.fileName)
    dataPath = Path.combine(dataPath, "6.txt")

    let lines = assume File.readAllLines(dataPath)

    let first  = Tokenize.split(lines[0], `:`)
    let second = Tokenize.split(lines[1], `:`)
    let val0   = Tokenize.split(first[1], ` `)
    let val1   = Tokenize.split(second[1], ` `)

    var times: Array's32
    var dists: Array's32

    visit v: val0 times.add(assume StrConv.toS32(v))
    visit v: val1 dists.add(assume StrConv.toS32(v))

    var total: u64 = 1
    visit t, idx: times
    {
        var cpt = 0
        loop i: t
        {
            let travel = (t - i) * i
            if travel > dists[idx]
                cpt += 1
        }

        total *= cpt
    }

    //Console.print("total: ", total)
    @assert(total == 281600)
}

