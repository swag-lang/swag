/*
--- Part Two ---
Now, let's go the other way. In addition to finding the number of characters of code, you should now encode each code representation as a new string and find the number of characters of the new encoded representation, including the surrounding double quotes.

For example:

"" encodes to "\"\"", an increase from 2 characters to 6.
"abc" encodes to "\"abc\"", an increase from 5 characters to 9.
"aaa\"aaa" encodes to "\"aaa\\\"aaa\"", an increase from 10 characters to 16.
"\x27" encodes to "\"\\x27\"", an increase from 6 characters to 11.
Your task is to find the total number of characters to represent the newly encoded strings minus the number of characters of code in each original string literal. For example, for the strings above, the total encoded length (6 + 9 + 16 + 11 = 42) minus the characters in the original code representation (23, just like in the first part of this puzzle) is 42 - 23 = 19.
*/

using Core

func day8B()
{
    //Time.Stopwatch.scopeMeasure()

    let dataPath = Path.combine(Path.getDirectoryName(#file), "8.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total0, total1: s64
    foreach l in lines
    {
        var i = 0
        while i < l.length
        {
            if i != l.length - 1 and l[i] == '\\' and l[i + 1] == '"'
            {
                total1 += 4
                i += 2
            }
            elif i < l.length - 1 and l[i] == '\\' and l[i + 1] == '\\'
            {
                total1 += 4
                i += 2
            }
            elif l.length > 3 and i < l.length - 3 and l[i] == '\\' and l[i + 1] == 'x' and Latin1.isHexDigit(l[i + 2]) and Latin1.isHexDigit(l[i + 3])
            {
                total1 += 5
                i += 4
            }
            else
            {
                total1 += 1
                i += 1
            }
        }

        //Console.print(l, " => ", res, "\n")
        total0 += l.length
        total1 += 4
    }

    //Console.print("total: ", total1-total0)
    @assert(total1 - total0 == 2046)
}
