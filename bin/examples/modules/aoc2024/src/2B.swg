/*
--- Part Two ---
The engineers are surprised by the low number of safe reports until they realize they forgot to tell you about the Problem Dampener.

The Problem Dampener is a reactor-mounted module that lets the reactor safety systems tolerate a single bad level in what would otherwise be a safe report. It's like the bad level never happened!

Now, the same rules apply as before, except if removing a single level from an unsafe report would make it safe, the report instead counts as safe.

More of the above example's reports are now safe:

7 6 4 2 1: Safe without removing any level.
1 2 7 8 9: Unsafe regardless of which level is removed.
9 7 6 2 1: Unsafe regardless of which level is removed.
1 3 2 4 5: Safe by removing the second level, 3.
8 6 4 4 1: Safe by removing the third level, 4.
1 3 6 7 9: Safe without removing any level.
Thanks to the Problem Dampener, 4 reports are actually safe!

Update your analysis by handling situations where the Problem Dampener can remove a single level from unsafe reports. How many reports are now safe?
*/

using Core

func day2B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "2.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total = 0
    foreach l in lines
    {
        let first0 = Tokenize.split(l, ' ''u8)
        var first  = first0

        var tor = 0
        for
        {
            var prev = 0, ls = 0
            var bad  = false
            foreach v in first
            {
                let a = assume StrConv.toS32(v)
                if @index
                {
                    let diff = a - prev
                    if @abs(diff) < 1 or @abs(diff) > 3 do
                        bad = true
                    if ls and Math.sign(diff) != ls do
                        bad = true
                    ls = Math.sign(diff)
                }

                prev = a
            }

            if bad
            {
                if tor == first0.count do
                    break
                first = first0
                first.removeAtOrdered(tor)
                tor += 1
                continue
            }

            total += 1
            break
        }
    }

    @assert(total == 589)
}
