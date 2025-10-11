/*
--- Part Two ---
To make things a little more interesting, the Elf introduces one additional rule. Now, J cards are jokers - wildcards that can act like whatever card would make the hand the strongest type possible.

To balance this, J cards are now the weakest individual cards, weaker even than 2. The other cards stay in the same order: A, K, Q, T, 9, 8, 7, 6, 5, 4, 3, 2, J.

J cards can pretend to be whatever card is best for the purpose of determining hand type; for example, QJJQ2 is now considered four of a kind. However, for the purpose of breaking ties between two hands of the same type, J is always treated as J, not the card it's pretending to be: JKKK2 is weaker than QQQQ2 because J is weaker than Q.

Now, the above example goes very differently:

32T3K 765
T55J5 684
KK677 28
KTJJT 220
QQQJA 483
32T3K is still the only one pair; it doesn't contain any jokers, so its strength doesn't increase.
KK677 is now the only two pair, making it the second-weakest hand.
T55J5, KTJJT, and QQQJA are now all four of a kind! T55J5 gets rank 3, QQQJA gets rank 4, and KTJJT gets rank 5.
With the new joker rule, the total winnings in this example are 5905.

Using the new joker rule, find the rank of every hand in your set. What are the new total winnings?
*/

using Core

private struct Card
{
    val:        string
    bid:        u64
    count:      [13] s32
    countV:     [6] s32
    value:      s32
}

private var map: [256] u8
private var cards: Array'Card

private func sort()
{
    cards.sort(func(a, b)
    {
        if a.value != b.value do
            return a.value <=> b.value

        for i in a.val
        {
            let it0 = map[a.val[i]]
            let it1 = map[b.val[i]]
            if it0 != it1 do
                return it0 <=> it1
        }

        return 0
    })
}

func day7B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "7.txt")
    let lines    = assume File.readAllLines(dataPath)

    map['A'] = 12
    map['K'] = 11
    map['Q'] = 10
    map['T'] = 9
    map['9'] = 8
    map['8'] = 7
    map['7'] = 6
    map['6'] = 5
    map['5'] = 4
    map['4'] = 3
    map['3'] = 2
    map['2'] = 1
    map['J'] = 0

    foreach l in lines
    {
        let first = Tokenize.split(l, ' ''u8)

        var card:    Card
        var maxCount = 0
        foreach c in first[0]
        {
            let it = map[c]
            card.count[it] += 1
            if it do
                maxCount = Math.max(maxCount, card.count[it])
        }

        foreach c, i in card.count
        {
            if c and i do
                card.countV[c] += 1
        }

        card.countV[maxCount + card.count[0]] += 1
        card.countV[maxCount] -= 1

        switch
        {
        case card.countV[5]:
            card.value = 6
        case card.countV[4]:
            card.value = 5
        case card.countV[3] and card.countV[2]:
            card.value = 4
        case card.countV[3]:
            card.value = 3
        case card.countV[2] == 2:
            card.value = 2
        case card.countV[2]:
            card.value = 1
        }

        card.val = first[0]
        card.bid = assume StrConv.toU64(first[1])
        cards.add(card)
    }

    sort()

    var total: u64
    foreach c, idx in cards do
        total += (idx + 1) * c.bid

    //Console.print("total: ", total)
    @assert(total == 248029057)
}
