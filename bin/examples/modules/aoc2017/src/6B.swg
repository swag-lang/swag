/*
--- Part Two ---
Out of curiosity, the debugger would also like to know the size of the loop: starting from a state that has already been seen, how many block redistribution cycles must be performed before that same state is seen again?

In the example above, 2 4 1 2 is seen again after four cycles, and so the answer in that example would be 4.

How many cycles are in the infinite loop that arises from the configuration in your puzzle input?
*/

using Core

func day6B()
{
    let line   = "4 10 4 1 8 4 9 14 5 1 14 15 0 15 3 5"
    let slice  = Tokenize.split(line, " ")
    var blocks = Slice.map(slice[..], func(x) => assume StrConv.toS32(x))

    var set: HashTable'(String, u64)
    var total = 0'u64
    loop
    {
        var best, besti: u64
        visit v, i: blocks
        {
            if v > best
            {
                best = v
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
        loop i: blocks.count do
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
