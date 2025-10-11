/*
--- Part Two ---
Realizing the error of his ways, Santa has switched to a better model of determining whether a string is naughty or nice. None of the old rules apply, as they are all clearly ridiculous.

Now, a nice string is one with all of the following properties:

It contains a pair of any two letters that appears at least twice in the string without overlapping, like xyxy (xy) or aabcdefgaa (aa), but not like aaa (aa, but it overlaps).
It contains at least one letter which repeats with exactly one letter between them, like xyx, abcdefeghi (efe), or even aaa.
For example:

qjhvhtzxzqqjkmpb is nice because is has a pair that appears twice (qj) and a letter that repeats with exactly one letter between them (zxz).
xxyxx is nice because it has a pair that appears twice and a letter that repeats with one between, even though the letters used by each rule overlap.
uurcxstgmygtbstg is naughty because it has a pair (tg) but no repeat with a single letter between them.
ieodomkazucvgmuy is naughty because it has a repeating letter with one between (odo), but no pair that appears twice.
How many strings are nice under these new rules?
*/

using Core

func day5B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "5.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total = 0
    foreach l in lines
    {
        var dbl0, dbl1 = 0
        for i in l.length - 2
        {
            if Utf8.indexOf(l[i + 2 to ], l[i to i + 1]) != Swag.U64.Max do
                dbl0 += 1
            if l[i] == l[i + 2] do
                dbl1 += 1
        }

        if dbl0 and dbl1 do
            total += 1
    }

    //Console.print("total: ", total)
    @assert(total == 53)
}
