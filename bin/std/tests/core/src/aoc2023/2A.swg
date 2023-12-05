// https://adventofcode.com/ 
// Year 2023

using Core

#test
{
    var dataPath: String = Path.getDirectoryName(#location.fileName)
    dataPath = Path.combine(dataPath, "2.txt")

    let lines = assume File.readAllLines(dataPath)
    var total = 0'u64

    visit l, gameIdx: lines
    {    
        let first = Tokenize.split(l, `:`)
        let sets  = Tokenize.split(first[1], `;`)

        struct RGB
        {
            r, g, b: s32
        }

        var game: RGB
        visit s: sets
        {
            let values = Tokenize.split(s, `,`)
            visit v: values
            {
                let one = Tokenize.split(v, ` `)
                switch one[1]
                {
                case "red":   game.r = Math.max(game.r, assume StrConv.toS32(one[0]))
                case "green": game.g = Math.max(game.g, assume StrConv.toS32(one[0]))
                case "blue":  game.b = Math.max(game.b, assume StrConv.toS32(one[0]))
                }
            }
        }

        if game.r <= 12 and game.g <= 13 and game.b <= 14
            total += gameIdx + 1
    }

    //Console.print("total: ", total)
    @assert(total == 2369)
}

