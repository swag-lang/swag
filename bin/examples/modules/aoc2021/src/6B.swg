/*
--- Part Two ---
Suppose the lanternfish live forever and have unlimited food and space. Would they take over the entire ocean?

After 256 days in the example above, there would be a total of 26984457539 lanternfish!

How many lanternfish would there be after 256 days?
*/

using Core

func day6B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "6.txt")
    let lines    = assume File.readAllLines(dataPath)
    let str      = Tokenize.split(lines[0], ',''u8)

    var state: [9] u64
    foreach v in str do
        state[assume StrConv.toS32(v)] += 1

    var total: u64
    for 256
    {
        var oldState = state
        Memory.clear(&state[0], #sizeof(state))

        state[6], state[8] = oldState[0]
        total = oldState[0] * 2

        foreach f, i in oldState[1 to ]
        {
            state[i] += oldState[i + 1]
            total += oldState[i + 1]
        }
    }

    //Console.print("total: ", total)
    @assert(total == 1741362314973)
}
