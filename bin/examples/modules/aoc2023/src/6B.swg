/*
--- Part Two ---
As the race is about to start, you realize the piece of paper with race times and record distances you got earlier actually just has very bad kerning. There's really only one race - ignore the spaces between the numbers on each line.

So, the example from before:

Time:      7  15   30
Distance:  9  40  200
...now instead means this:

Time:      71530
Distance:  940200
Now, you have to figure out how many ways there are to win this single race. In this example, the race lasts for 71530 milliseconds and the record distance you need to beat is 940200 millimeters. You could hold the button anywhere from 14 to 71516 milliseconds and beat the record, a total of 71503 ways!

How many ways can you beat the record in this one much longer race?
*/

using Core

func day6B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "6.txt")
    let lines    = assume File.readAllLines(dataPath)

    let first  = Tokenize.split(lines[0], ':''u8)
    let second = Tokenize.split(lines[1], ':''u8)
    let val0   = Tokenize.split(first[1], ' ''u8)
    let val1   = Tokenize.split(second[1], ' ''u8)

    var times: Array'u64
    var dists: Array'u64

    var str0, str1: String = ""
    foreach v in val0 do
        str0 += v
    foreach v in val1 do
        str1 += v

    times.add(assume StrConv.toU64(str0))
    dists.add(assume StrConv.toU64(str1))

    var total: u64 = 1
    foreach t, idx in times
    {
        var cpt = 0
        let dx  = dists[idx]
        for i in t
        {
            let travel = (t - i) * i
            if travel > dx do
                cpt += 1
        }

        total *= cpt
    }

    //Console.print("total: ", total)
    @assert(total == 33875953)
}
