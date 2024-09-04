/*
--- Part Two ---
The Elf finishes helping with the tent and sneaks back over to you. "Anyway, the second column says how the round needs to end: X means you need to lose, Y means you need to end the round in a draw, and Z means you need to win. Good luck!"

The total score is still calculated in the same way, but now you need to figure out what shape to choose so the round ends as indicated. The example above now goes like this:

In the first round, your opponent will choose Rock (A), and you need the round to end in a draw (Y), so you also choose Rock. This gives you a score of 1 + 3 = 4.
In the second round, your opponent will choose Paper (B), and you choose Rock so you lose (X) with a score of 1 + 0 = 1.
In the third round, you will defeat your opponent's Scissors with Rock for a score of 1 + 6 = 7.
Now that you're correctly decrypting the ultra top secret strategy guide, you would get a total score of 12.

Following the Elf's instructions for the second column, what would your total score be if everything goes exactly according to your strategy guide?
*/

using Core

func day2B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "2.txt")
    let lines    = assume File.readAllLines(dataPath)

    const Score: [3, 3] s32 = [
        [1 + 3, 2 + 6, 3 + 0],
        [1 + 0, 2 + 3, 3 + 6],
        [1 + 6, 2 + 0, 3 + 3]]

    const Swap: [3, 3] u8 = [
        [2, 0, 1],
        [0, 1, 2],
        [1, 2, 0]]

    var total: u64
    foreach l in lines
    {
        let val1 = l[0] - 'A'
        var val2 = l[2] - 'X'
        val2 = Swap[val1, val2]
        total += Score[val1, val2]
    }

    //Console.print("total: ", total)
    @assert(total == 12091)
}
