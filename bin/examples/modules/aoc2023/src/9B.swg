/*
--- Part Two ---
Of course, it would be nice to have even more history included in your report. Surely it's safe to just extrapolate backwards as well, right?

For each history, repeat the process of finding differences until the sequence of differences is entirely zero. Then, rather than adding a zero to the end and filling in the next values of each previous sequence, you should instead add a zero to the beginning of your sequence of zeroes, then fill in new first values for each previous sequence.

In particular, here is what the third example history looks like when extrapolating back in time:

5  10  13  16  21  30  45
  5   3   3   5   9  15
   -2   0   2   4   6
      2   2   2   2
        0   0   0
Adding the new values on the left side of each sequence from bottom to top eventually reveals the new left-most history value: 5.

Doing this for the remaining example data above results in previous values of -3 for the first history and 0 for the second history. Adding all three new values together produces 2.

Analyze your OASIS report again, this time extrapolating the previous value for each history. What is the sum of these extrapolated values?
*/

using Core

func day9B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "9.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total: s64

    foreach l in lines
    {
        let vals = Tokenize.split(l, ' ''u8)

        var values: Array's64
        foreach v in vals do
            values.add(assume StrConv.toS64(v))

        var column: Array's64
        column.add(values[0])

        var recom = true
        while recom
        {
            recom = false
            var newValues: Array's64
            for i in 0 until values.count - 1
            {
                let v = values[i + 1] - values[i]
                recom = v or recom
                newValues.add(v)

                if i == 0 do
                    column.add(v)
            }

            values = newValues
        }

        var prev: s64
        foreach #reverse c in column
        {
            let c1 = c - prev
            prev = c1
        }

        total += prev
    }

    //Console.print("total: ", total)
    @assert(total == 1019)
}
