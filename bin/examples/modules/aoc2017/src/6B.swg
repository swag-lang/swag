/*
--- Part Two ---
Out of curiosity, the debugger would also like to know the size of the for: starting from a state that has already been seen, how many block redistribution cycles must be performed before that same state is seen again?

In the example above, 2 4 1 2 is seen again after four cycles, and so the answer in that example would be 4.

How many cycles are in the infinite for that arises from the configuration in your puzzle input?
*/

using Core

func day6B()
{
    let line   = "4 10 4 1 8 4 9 14 5 1 14 15 0 15 3 5"
    let slice  = Tokenize.split(line, " ")
    var blocks = Slice.map(slice[ to ], func(x) => assume StrConv.toS32(x))

    var set:  HashTable'(String, u64)
    var total = 0'u64
    for
    {
        var best, besti: u64
        foreach v, i in blocks
        {
            if v > best
            {
                best  = v
                besti = i
            }
        }

        blocks[besti] = 0
        while best
        {
            besti = (besti + 1) % blocks.count
            blocks[besti] += 1
            best -= 1
        }

        var val: String
        for i in blocks.count do
            val += Format.toString("%.", blocks[i])

        total += 1
        if set.contains(val)
        {
            total -= set[val]
            break
        }

        set[val] = total
    }

    @assert(total == 8038)
}
