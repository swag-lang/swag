/*
--- Part Two ---
Suppose the lanternfish live forever and have unlimited food and space. Would they take over the entire ocean?

After 256 days in the example above, there would be a total of 26984457539 lanternfish!

How many lanternfish would there be after 256 days?
*/

using Core

func day7A()
{
    let dataPath = Path.combine(Path.getDirectoryName(#location.fileName), "7.txt")
    let lines = assume File.readAllLines(dataPath)
    let str = Tokenize.split(lines[0], `,`'u8)

    var crabs: Array's32
    visit v: str
        crabs.add(assume StrConv.toS32(v))

    crabs.sort()

    // https://en.wikipedia.org/wiki/Geometric_median#Properties
    var total = 0
    let median = crabs[crabs.count/2]
    visit v: crabs
        total += Math.abs(v-median)

    //Console.print("total: ", total)
    @assert(total == 342730)
}

