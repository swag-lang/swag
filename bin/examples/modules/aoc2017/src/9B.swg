/*
--- Part Two ---
Now, you're ready to remove the garbage.

To prove you've removed it, you need to count all of the characters within the garbage. The leading and trailing < and > don't count, nor: any canceled characters or the ! doing the canceling.

<>, 0 characters.
<random characters>, 17 characters.
<<<<>, 3 characters.
<{!>}>, 2 characters.
<!!>, 0 characters.
<!!!>>, 0 characters.
<{o"i!a,<{i<a>, 10 characters.
How many non-canceled characters are within the garbage in your puzzle input?
*/

using Core

func day9B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "9.txt")
    let line     = assume File.readAllText(dataPath)

    var garbage, group, score, count = 0
    for var i = 0; i < @countof(line); i += 1
    {
        switch
        {
        case line[i] == '!' and garbage:
            i += 1
        case line[i] == '>' and garbage:
            garbage -= 1
        case line[i] == '<' and !garbage:
            garbage += 1
        case line[i] == '{' and !garbage:
            group += 1; score += group
        case line[i] == '}' and !garbage:
            group -= 1
        case garbage:
            count += 1
        }
    }

    @assert(count == 5601)
}
