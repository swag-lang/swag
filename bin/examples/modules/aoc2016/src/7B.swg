/*
--- Part Two ---
You would also like to know which IPs support SSL (super-secret listening).

An IP supports SSL if it has an Area-Broadcast Accessor, or ABA, anywhere in the supernet sequences (outside any square bracketed sections), and a corresponding Byte Allocation Block, or BAB, anywhere in the hypernet sequences. An ABA is any three-character sequence which consists of the same character twice with a different character between them, such as xyx or aba. A corresponding BAB is the same characters but in reversed positions: yxy and bab, respectively.

For example:

aba[bab]xyz supports SSL (aba outside square brackets with corresponding bab within square brackets).
xyx[xyx]xyx does not support SSL (xyx, but no corresponding yxy).
aaa[kek]eke supports SSL (eke in supernet with corresponding kek in hypernet; the aaa sequence is not related, because the interior character must be different).
zazbz[bzb]cdb supports SSL (zaz has no corresponding aza, but zbz has a corresponding bzb, even though zaz and zbz overlap).
How many IPs in your puzzle input support SSL?
*/

using Core

func day7B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "7.txt")
    let lines    = assume File.readAllLines(dataPath)

    var total = 0
    foreach l in lines
    {
        l.replace("]", "[")

        var outside, inside: String
        foreach it, i in Tokenize.split(l, '[''u8)
        {
            if !(i & 1) do
                outside += it
            else do
                inside += it
        }

        for i in 0 until @countof(outside) - 2
        {
            if outside[i] == outside[i + 2] and
               outside[i] != outside[i + 1] and
               inside.contains([outside[i + 1], outside[i], outside[i + 1]])
            {
                total += 1
                break
            }
        }
    }

    //Console.print(total)
    @assert(total == 242)
}
