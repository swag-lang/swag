/*
--- Part Two ---
Suppose the lanternfish live forever and have unlimited food and space. Would they take over the entire ocean?

After 256 days in the example above, there would be a total of 26984457539 lanternfish!

How many lanternfish would there be after 256 days?
*/

using Core

func day6B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#location.fileName), "6.txt")
    let lines = assume File.readAllLines(dataPath)

    var values: Array's32
    let str = Tokenize.split(lines[0], `,`'u8)
    visit v: str values.add(assume StrConv.toS32(v))

    var state: [9] u64
    visit f: values
        state[f] += 1

    loop 256
    {
        var oldState = state
        Memory.clear(&state[0], @sizeof(state))

        visit f,i: oldState
        {
            if i == 0
                state[6], state[8] += f
            else
                state[i-1] += oldState[i]
        }
    }

    var total: u64
    visit f: state
        total += f
    //Console.print("total: ", total)
    @assert(total == 1741362314973)
}

