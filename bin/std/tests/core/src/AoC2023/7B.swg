// https://adventofcode.com/ 
// Year 2023

using Core

struct Card
{
    val:    string;
    bid:    u64
    count:  [13] s32
    countV: [6] s32
    value:  s32
}

var map:   [256] u8
var cards: Array'Card

func sort()
{
    cards.sort(func(a, b) {
        if a.value != b.value
            return a.value <=> b.value

        loop i: a.val
        {
            let it0 = map[a.val[i]]
            let it1 = map[b.val[i]]
            if it0 != it1
                return it0 <=> it1
        }

        return 0
    })
}

#test
{
    var dataPath: String = Path.getDirectoryName(#location.fileName)
    dataPath = Path.combine(dataPath, "7.txt")

    let lines = assume File.readAllLines(dataPath)

    map[`A`] = 12
    map[`K`] = 11
    map[`Q`] = 10
    map[`T`] = 9
    map[`9`] = 8
    map[`8`] = 7
    map[`7`] = 6
    map[`6`] = 5
    map[`5`] = 4
    map[`4`] = 3
    map[`3`] = 2
    map[`2`] = 1
    map[`J`] = 0

    visit l: lines
    {
        let first = Tokenize.split(l, ` `)

        var card: Card
        var maxCount = 0
        visit c: first[0]
        {
            let it = map[c]
            card.count[it] += 1
            if it
                maxCount = Math.max(maxCount, card.count[it])
        }

        visit c, i: card.count
        {
            if c and i
                card.countV[c] += 1
        }  

        card.countV[maxCount + card.count[0]] += 1
        card.countV[maxCount] -= 1

        switch
        {
        case card.countV[5]:                        card.value = 6
        case card.countV[4]:                        card.value = 5
        case card.countV[3] and card.countV[2]:     card.value = 4
        case card.countV[3]:                        card.value = 3            
        case card.countV[2] == 2:                   card.value = 2             
        case card.countV[2]:                        card.value = 1            
        }

        card.val = first[0]
        card.bid = assume StrConv.toU64(first[1])
        cards.add(card)
    }

    sort()

    var total: u64
    visit c, idx: cards
        total += (idx+1) * c.bid

    //Console.print("total: ", total)
    @assert(total == 248029057)
}

