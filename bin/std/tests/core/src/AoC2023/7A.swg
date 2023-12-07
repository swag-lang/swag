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
    map.add(`J`, 9)
    map.add(`T`, 8)
    map.add(`9`, 7)
    map.add(`8`, 6)
    map.add(`7`, 5)
    map.add(`6`, 4)
    map.add(`5`, 3)
    map.add(`4`, 2)
    map.add(`3`, 1)
    map.add(`2`, 0)

    visit l: lines
    {
        let first = Tokenize.split(l, ` `)

        var card: Card
        visit c: first[0]
        {
            let it = map.find(c)
            card.count[it.value] += 1
        }

        visit c: card.count
        {
            card.countV[c] += 1
        }

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
    @assert(total == 249748283)
}

