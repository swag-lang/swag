/*
--- Part Two ---
As you watch the crane operator expertly rearrange the crates, you notice the process isn't following your prediction.

Some mud was covering the writing on the side of the crane, and you quickly wipe it away. The crane isn't a CrateMover 9000 - it's a CrateMover 9001.

The CrateMover 9001 is notable for many new and exciting features: air conditioning, leather seats, an extra cup holder, and the ability to pick up and move multiple crates at once.

Again considering the example above, the crates begin in the same configuration:

    [D]
[N] [C]
[Z] [M] [P]
 1   2   3
Moving a single crate from stack 2 to stack 1 behaves the same as before:

[D]
[N] [C]
[Z] [M] [P]
 1   2   3
However, the action of moving three crates from stack 1 to stack 3 means that those three moved crates stay in the same order, resulting in this new configuration:

        [D]
        [N]
    [C] [Z]
    [M] [P]
 1   2   3
Next, as both crates are moved from stack 2 to stack 1, they retain their order as well:

        [D]
        [N]
[C]     [Z]
[M]     [P]
 1   2   3
Finally, a single crate is still moved from stack 1 to stack 2, but now it's crate C that gets moved:

        [D]
        [N]
        [Z]
[M] [C] [P]
 1   2   3
In this example, the CrateMover 9001 has put the crates in a totally different order: MCD.

Before the rearrangement process finishes, update your simulation so that the Elves know where they should stand to be ready to unload the final supplies. After the rearrangement procedure completes, what crate ends up on top of each stack?
*/

using Core

func day5B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "5.txt")
    let lines    = assume File.readAllLines(dataPath)

    var stacks:  Array'String
    var r:       Parser.RegExp
    var moveMode = false
    foreach l in lines
    {
        if moveMode
        {
            if !catch r.match(#"move (\d+) from (\d+) to (\d+)"#, l) do
                continue
            let count   = assume StrConv.toU64(r.getCapture(1))
            let colFrom = assume StrConv.toS32(r.getCapture(2)) - 1
            let colTo   = assume StrConv.toS32(r.getCapture(3)) - 1

            let str   = #ref stacks[colFrom]
            let slice = str[0 until count]
            stacks[colTo].insert(0, slice)
            str.remove(0, count)
        }
        elif l[1] == '1'
        {
            moveMode = true
        }
        else
        {
            var stack = 0
            for var i = 0; i < l.length; i += 4
            {
                if stack >= stacks.count do
                    stacks.add("")
                if l[i] == '[' do
                    stacks[stack].append(l[i + 1])
                stack += 1
            }
        }
    }

    var total: String
    foreach s in stacks do
        total += s[0]

    //Console.print("total: ", total)
    @assert(total == "RNLFDJMCT")
}
