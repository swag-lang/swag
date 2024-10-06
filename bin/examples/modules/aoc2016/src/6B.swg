/*
--- Part Two ---
Of course, that would be the message - if you hadn't agreed to use a modified repetition code instead.

In this modified code, the sender instead transmits what looks like random data, but for each character, the character they actually want to send is slightly less likely than the others. Even after signal-jamming noise, you can look at the letter distributions in each column and choose the least common letter to reconstruct the original message.

In the above example, the least common character in the first column is a; in the second, d, and so on. Repeating this process for the remaining characters produces the original message, advent.

Given the recording in your puzzle input and this new decoding methodology, what is the original message that Santa is trying to send?
*/

using Core

func day6B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "6.txt")
    let lines    = assume File.readAllLines(dataPath)

    var freq: [8, 26] u32
    var pwd:  String = "        "

    foreach l in lines
    {
        foreach c, i in l
        {
            freq[i, c - 'a'] += 1
        }
    }

    var best: [8] u32 = Swag.U32.Max
    for c0 in 8
    {
        for c1 in 26
        {
            let v = freq[c0, c1]
            if v and v < best[c0]
            {
                best[c0] = v
                pwd[c0]  = cast(u8) (c1 + 'a')
            }
        }
    }

    //Console.print(pwd)
    @assert(pwd == "bhkzekao")
}
