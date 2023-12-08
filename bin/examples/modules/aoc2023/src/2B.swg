// https://adventofcode.com/ 
// Year 2023

using Core

func day2B()
{
    var dataPath: String = Path.getDirectoryName(#location.fileName)
    dataPath = Path.combine(dataPath, "2.txt")

    let lines = assume File.readAllLines(dataPath)
    var total = 0'u64

    visit l, gameIdx: lines
    {    
        let first = Tokenize.split(l, `:`)
        let sets  = Tokenize.split(first[1], `;`)

        var game: {r, g, b: s32}
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

        total += game.r * game.g * game.b
    }

    //Console.print("total: ", total)
    @assert(total == 66363)
}

