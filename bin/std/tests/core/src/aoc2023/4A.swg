// https://adventofcode.com/ 
// Year 2023

using Core

#test
{
    var dataPath: String = Path.getDirectoryName(#location.fileName)
    dataPath = Path.combine(dataPath, "4.txt")

    let lines = assume File.readAllLines(dataPath)

    var total = 0'u64
    visit l: lines
    {
        let first = Tokenize.split(l, `:`)
        let game  = Tokenize.split(first[1], `|`)
        let val0  = Tokenize.split(game[0], ` `)
        let val1  = Tokenize.split(game[1], ` `)

        var v0, v1: Array's32
        visit v: val0 v0.add(assume StrConv.toS32(v))
        visit v: val1 v1.add(assume StrConv.toS32(v))

        var score = 1
        visit v: v1
        {
            if v0.contains(v)
                score <<= 1
        }

        total += score/2
    }

    //Console.print("total: ", total)
    @assert(total == 23673)
}

