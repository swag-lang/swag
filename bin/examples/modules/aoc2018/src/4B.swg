/*
--- Part Two ---
Strategy 2: Of all guards, which guard is most frequently asleep on the same minute?

In the example above, Guard #99 spent minute 45 asleep more than any other guard or minute - three times in total. (In all other cases, any guard spent any minute asleep at most twice.)

What is the ID of the guard you chose multiplied by the minute you chose? (In the above example, the answer would be 99 * 45 = 4455.)
*/

using Core, Core.Parser

func day4B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "4.txt")
    let lines    = assume File.readAllLines(dataPath)
    Slice.sort(lines[ to ])

    var curGuard, startSleep, curMin = 0
    var count: [60] HashTable'(s32, s32)

    foreach l in lines
    {
        curMin = assume StrConv.parseS32(l[15 to ]).value

        if Utf8.contains(l, "begins shift") do
            curGuard = assume StrConv.parseS32(l[26 to ]).value
        elif Utf8.contains(l, "falls asleep") do
            startSleep = curMin
        elif Utf8.contains(l, "wakes up") do
            for i in startSleep until curMin do
                count[i].findOrAdd(curGuard).value += 1
    }

    var minCount, min, guard = 0'u64
    foreach v, i in count
    {
        foreach k1, v1 in v
        {
            if v1 > minCount
            {
                minCount = v1
                min      = i
                guard    = k1
            }
        }
    }

    //Console.print("total: ", guard*min)
    @assert(guard * min == 10491)
}
