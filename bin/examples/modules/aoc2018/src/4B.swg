/*
--- Part Two ---
Strategy 2: Of all guards, which guard is most frequently asleep on the same minute?

In the example above, Guard #99 spent minute 45 asleep more than any other guard or minute - three times in total. (In all other cases, any guard spent any minute asleep at most twice.)

What is the ID of the guard you chose multiplied by the minute you chose? (In the above example, the answer would be 99 * 45 = 4455.)
*/

using Core, Core.Parser

func day4B() assume
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "4.txt")
    let lines    = File.readAllLines(dataPath)
    Slice.sort(lines[..])

    var curGuard, startSleep, curMin = 0
    var count: [60] HashTable'(s32, s32)

    visit l: lines
    {
        curMin = StrConv.parseS32(l[15..]).value

        if Utf8.contains(l, "begins shift") do
            curGuard = StrConv.parseS32(l[26..]).value
        elif Utf8.contains(l, "falls asleep") do
            startSleep = curMin
        elif Utf8.contains(l, "wakes up") do
            loop i: startSleep until curMin do
                count[i].findOrAdd(curGuard).value += 1
    }

    var minCount, min, guard = 0'u64
    visit v, i: count
    {
        visit k1, v1: v
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
