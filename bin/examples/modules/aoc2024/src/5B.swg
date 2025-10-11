/*
--- Part Two ---
While the Elves get to work printing the correctly-ordered updates, you have a little time to fix the rest of them.

For each of the incorrectly-ordered updates, use the page ordering rules to put the page numbers in the right order. For the above example, here are the three incorrectly-ordered updates and their correct orderings:

75,97,47,61,53 becomes 97,75,47,61,53.
61,13,29 becomes 61,29,13.
97,13,75,29,47 becomes 97,75,47,29,13.
After taking only the incorrectly-ordered updates and ordering them correctly, their middle page numbers are 47, 29, and 47. Adding these together produces 123.

Find the updates which are not in the correct order. What do you get if you add up the middle page numbers after correctly ordering just those updates?
*/

using Core
private var order: HashTable'(s32, Array's32)

func day5B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "5.txt")
    let lines    = assume File.readAllLines(dataPath)

    var first = true
    var total = 0

    foreach f in lines
    {
        if @countof(f) == 0
        {
            first = false
            continue
        }

        if first
        {
            let split = Tokenize.split(f, '|''u8)
            order.findOrAdd(assume StrConv.toS32(split[0])).value.add(assume StrConv.toS32(split[1]))
        }
        else
        {
            var split = Slice.map(Tokenize.split(f, ',''u8).toSlice(), func(x) => assume StrConv.toS32(x))

            var ok = true
            for i in split
            {
                let v0 = split[i]

                for j in i + 1 until split.count
                {
                    let v1 = split[j]

                    if let r0 = order.tryFind(v0) where !r0.value.contains(v1)
                    {
                        ok = false
                        break
                    }

                    if let r1 = order.tryFind(v1) where r1.value.contains(v0)
                    {
                        ok = false
                        break
                    }
                }
            }

            if !ok
            {
                split.sort(func(x, y)
                {
                    if let r0 = order.tryFind(x) where r0.value.contains(y) do
                        return -1
                    if let r1 = order.tryFind(y) where r1.value.contains(x) do
                        return 1

                    return x <=> y
                })

                total += split[split.count / 2]
            }
        }
    }

    @assert(total == 6767)
}
