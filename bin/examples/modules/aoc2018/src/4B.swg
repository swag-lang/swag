/*
--- Part Two ---
Strategy 2: Of all guards, which guard is most frequently asleep on the same minute?

In the example above, Guard #99 spent minute 45 asleep more than any other guard or minute - three times in total. (In all other cases, any guard spent any minute asleep at most twice.)

What is the ID of the guard you chose multiplied by the minute you chose? (In the above example, the answer would be 99 * 45 = 4455.)
*/

using Core, Parser

func day4B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "4.txt")
    let lines = assume File.readAllLines(dataPath)

    Slice.sort(lines[..], func(x, y)->s32
    {
        let val0 = assume Time.DateTime.parse(x[1..16], Time.DateTimeFormat.IsoHM)
        let val1 = assume Time.DateTime.parse(y[1..16], Time.DateTimeFormat.IsoHM)
        return val0.value <=> val1.value
    })

    var when = assume RegExp.create("(\\d+)-(\\d+)-(\\d+) (\\d+):(\\d+)")
    var start = assume RegExp.create("Guard #(\\d+) begins shift")
    var asleep = assume RegExp.create("falls asleep")
    var wakesup = assume RegExp.create("wakes up")

    var curGuard, startSleep, curMin = 0
    var count: [60] HashTable'(s32, s32)
    var total: HashTable'(s32, s32)

    visit l: lines
    {
        discard when.grep(l)
        curMin = assume StrConv.toS32(when[5])

        let s = start.grep(l)
        if s
        {
            curGuard = assume StrConv.toS32(start[1])
        }
        elif asleep.grep(l)
        {
            startSleep = curMin
        }
        elif wakesup.grep(l)
        {
            while startSleep < curMin
            {
                count[startSleep].findOrAdd(curGuard).value += 1
                total.findOrAdd(curGuard).value += 1
                startSleep += 1
            }
        }
    }

    var minCount, min, guard = 0'u64
    visit v, i: count
    {
        visit k1, v1: v
        {
            if v1 > minCount
            {
                minCount = v1
                min = i
                guard = k1
            }
        }
    }

    //Console.print("total: ", guard*min)
    @assert(guard*min == 10491)
}
