/*
--- Part Two ---
Neat, right? You might also enjoy hearing John Conway talking about this sequence (that's Conway of Conway's Game of Life fame).

Now, starting again with the digits in your puzzle input, apply this process 50 times. What is the length of the new result?
*/

using Core

func day10B()
{
    var l: String = "1113122113"
    for 50
    {
        var str: String
        var i    = 0
        while i < l.length
        {
            str += '1''u8
            str += l[i]
            i += 1
            while i < l.length and l[i] == str[str.length - 1]
            {
                str[str.length - 2] += 1
                i += 1
            }
        }

        l = str
    }

    //Console.print(l.length)
    @assert(l.length == 5103798)
}
