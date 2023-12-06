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

    var times: Array'u64
    var dists: Array'u64

    var str0, str1: String
    visit v: val0 str0 += v
    visit v: val1 str1 += v

    times.add(assume StrConv.toU64(str0))
    dists.add(assume StrConv.toU64(str1))

    var total, max: u64 = 1
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
    @assert(total == 33875953)
}

