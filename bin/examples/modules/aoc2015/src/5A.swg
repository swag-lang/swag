/*
--- Day 5: Doesn't He Have Intern-Elves For This? ---
Santa needs help figuring out which strings in his text file are naughty or nice.

A nice string is one with all of the following properties:

It contains at least three vowels (aeiou only), like aei, xazegov, or aeiouaeiouaeiou.
It contains at least one letter that appears twice in a row, like xx, abcdde (dd), or aabbccdd (aa, bb, cc, or dd).
It does not contain the strings ab, cd, pq, or xy, even if they are part of one of the other requirements.
For example:

ugknbfddgicrmopn is nice because it has at least three vowels (u...i...o...), a double letter (...dd...), and none of the disallowed substrings.
aaa is nice because it has at least three vowels and a double letter, even though the letters used by different rules overlap.
jchzalrnumimnmhp is naughty because it has no double letter.
haegwjzuvuyypxyu is naughty because it contains the string xy.
dvszwmarrgswjxmb is naughty because it contains only one vowel.
How many strings are nice?
*/

using Core

func day5A()
{
    day5A0()
    day5A1()
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day5A0()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "5.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total = 0
    foreach l in lines
    {
        if Utf8.indexOf(l, "ab") != Swag.U64.Max do
            continue
        if Utf8.indexOf(l, "cd") != Swag.U64.Max do
            continue
        if Utf8.indexOf(l, "pq") != Swag.U64.Max do
            continue
        if Utf8.indexOf(l, "xy") != Swag.U64.Max do
            continue

        var vowels, dbl = 0
        for i in l
        {
            if Utf8.indexOf("aeiou", cast(rune) l[i]) != Swag.U64.Max do
                vowels += 1
            if i and l[i] == l[i - 1] do
                dbl += 1
        }

        if vowels >= 3 and dbl do
            total += 1
    }

    //Console.print("total: ", total)
    @assert(total == 258)
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day5A1()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "5.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total = 0
    var r0    = assume Parser.RegExp.create("ab|cd|pq|xy")
    var r1    = assume Parser.RegExp.create("[aeiou]")
    var r2    = assume Parser.RegExp.create(#"([a-z])\1"#)
    foreach l in lines
    {
        if r0.grep(l) do
            continue
        if r1.grepAll(l).count < 3 do
            continue
        if !r2.grep(l) do
            continue
        total += 1
    }

    //Console.print("total: ", total)
    @assert(total == 258)
}
