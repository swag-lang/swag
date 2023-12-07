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

var map: HashTable'(u8, u8)
var cards: Array'Card

func sort()
{
    cards.sort(func(a, b) {
        if a.value != b.value
            return a.value <=> b.value

        loop i: a.val
        {
            let it0 = map.find(a.val[i])
            let it1 = map.find(b.val[i])
            if it0.value != it1.value
                return it0.value <=> it1.value
        }

        return 0
    })
}

#test
{
    var dataPath: String = Path.getDirectoryName(#location.fileName)
    dataPath = Path.combine(dataPath, "7.txt")

    let lines = assume File.readAllLines(dataPath)

    map.add(`A`, 12)
    map.add(`K`, 11)
    map.add(`Q`, 10)
    map.add(`T`, 9)
    map.add(`9`, 8)
    map.add(`8`, 7)
    map.add(`7`, 6)
    map.add(`6`, 5)
    map.add(`5`, 4)
    map.add(`4`, 3)
    map.add(`3`, 2)
    map.add(`2`, 1)
    map.add(`J`, 0)

    visit l: lines
    {
        let first = Tokenize.split(l, ` `)

        var card: Card
        var maxCount = 0
        visit c: first[0]
        {
            let it = map.find(c)
            card.count[it.value] += 1
            if it.value
                maxCount = Math.max(maxCount, card.count[it.value])
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
    {
        total += (idx+1) * c.bid
    }

    //Console.print("total: ", total)
    @assert(total == 248029057)
}

