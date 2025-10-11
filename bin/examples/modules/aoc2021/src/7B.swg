/*
--- Part Two ---
The crabs don't seem interested in your proposed solution. Perhaps you misunderstand crab engineering?

As it turns out, crab submarine engines don't burn fuel at a constant rate. Instead, each change of 1 step in horizontal position costs 1 more unit of fuel than the last: the first step costs 1, the second step costs 2, the third step costs 3, and so on.

As each crab moves, moving further becomes more expensive. This changes the best horizontal position to align them all on; in the example above, this becomes 5:

Move from 16 to 5: 66 fuel
Move from 1 to 5: 10 fuel
Move from 2 to 5: 6 fuel
Move from 0 to 5: 15 fuel
Move from 4 to 5: 1 fuel
Move from 2 to 5: 6 fuel
Move from 7 to 5: 3 fuel
Move from 1 to 5: 10 fuel
Move from 2 to 5: 6 fuel
Move from 14 to 5: 45 fuel
This costs a total of 168 fuel. This is the new cheapest possible outcome; the old alignment position (2) now costs 206 fuel instead.

Determine the horizontal position that the crabs can align to using the least fuel possible so they can make you an escape route! How much fuel must they spend to align to that position?
*/

using Core

// ********************************************
// For the record, i've cheated for this one !!
// ********************************************

func day7B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "7.txt")
    let lines    = assume File.readAllLines(dataPath)
    let str      = Tokenize.split(lines[0], ',''u8)

    var crabs: Array's32
    var mean:  s32
    foreach v in str
    {
        let c = assume StrConv.toS32(v)
        mean += c
        crabs.add(c)
    }

    if crabs.count do
        mean = cast(s32) (mean / cast(f32) crabs.count)

    var total = 0
    foreach v in crabs
    {
        let n = Math.abs(v - mean)
        total += (n * (n + 1)) / 2
    }

    //Console.print("total: ", total)
    @assert(total == 92335207)
}
