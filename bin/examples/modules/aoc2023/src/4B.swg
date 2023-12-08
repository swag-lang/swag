// https://adventofcode.com/ 
// Year 2023

using Core

private struct Card
{
    win: s32
    num: s32
}

private var cards: Array'Card

func day4B()
{
    var dataPath: String = Path.getDirectoryName(#location.fileName)
    dataPath = Path.combine(dataPath, "4.txt")

    let lines = assume File.readAllLines(dataPath)

    visit l: lines
    {
        let first = Tokenize.split(l, `:`)
        let game  = Tokenize.split(first[1], `|`)
        let val0  = Tokenize.split(game[0], ` `)
        let val1  = Tokenize.split(game[1], ` `)

        var v0, v1: Array's32
        visit v: val0 v0.add(assume StrConv.toS32(v))
        visit v: val1 v1.add(assume StrConv.toS32(v))

        var score = 0
        visit v: v1
        {
            if v0.contains(v)
                score += 1
        }

        cards.add({win: score, num: 1})
    }

    var total = 0'u64
    visit c, i: cards
    {
        loop x: c.win
        {
            cards[i + x + 1].num += c.num
        }

        total += c.num
    }

    //Console.print("total: ", total)
    @assert(total == 12263631)
}

