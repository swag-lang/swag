/*
--- Part Two ---
By the time you calculate the answer to the Elves' question, they've already realized that the Elf carrying the most Calories of food might eventually run out of snacks.

To avoid this unacceptable situation, the Elves would instead like to know the total Calories carried by the top three Elves carrying the most Calories. That way, even if one of those Elves runs out of snacks, they still have two backups.

In the example above, the top three Elves are the fourth Elf (with 24000 Calories), then the third Elf (with 11000 Calories), then the fifth Elf (with 10000 Calories). The sum of the Calories carried by these three elves is 45000.

Find the top three Elves carrying the most Calories. How many Calories are those Elves carrying in total?
*/

using Core

func day1B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "1.txt")
    let lines    = assume File.readAllLines(dataPath)

    var count: [3] u64
    var one:   u64
    foreach l in lines
    {
        one += catch StrConv.toU64(l)
        if l.isEmpty() or @index == lines.count - 1
        {
            switch
            {
            case one > count[0]:
                count[0] = one
            case one > count[1]:
                count[1] = one
            case one > count[2]:
                count[2] = one
            }

            Slice.sort(count[ to ])
            one = 0
        }
    }

    let total = count[0] + count[1] + count[2]
    //Console.print("total: ", total)
    @assert(total == 203002)
}
