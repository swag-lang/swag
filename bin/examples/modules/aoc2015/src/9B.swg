/*
--- Part Two ---
The next year, just to show off, Santa decides to take the route with the longest distance instead.

He can still start and end at any two (different) locations he wants, and he still must foreach each location exactly once.

For example, given the distances above, the longest route would be 982 via (for example) Dublin -> London -> Belfast.

What is the distance of the longest route?
*/

using Core

func day9B()
{
    //Time.Stopwatch.scopeMeasure()

    let dataPath = Path.combine(Path.getDirectoryName(#file), "9.txt")
    let lines    = assume File.readAllLines(dataPath)

    var r      = assume Parser.RegExp.create(#"(\w+) to (\w+) = (\d+)"#)
    var town:  HashTable'(string, u64)
    var dist:  HashTable'(u64, u64)
    var combi: Array'u64

    foreach l in lines
    {
        discard r.match(l)
        town.add(r[1], town.count, updateValue: false)
        town.add(r[2], town.count, updateValue: false)
        dist.add(town.find(r[1]).value << 32 | town.find(r[2]).value, assume StrConv.toU64(r[3]))
        dist.add(town.find(r[2]).value << 32 | town.find(r[1]).value, assume StrConv.toU64(r[3]))
    }

    for i in town.count do
        combi.add(i)

    var total = 0'u64
    for
    {
        var calc = 0'u64
        for i in cast(s64) combi.count - 1 do
            calc += dist.find(combi[i] << 32 | combi[i + 1]).value
        total = Math.max(total, calc)
        if !Slice.nextPermutation(combi.toSlice()) do
            break
    }

    //Console.print("total: ", total)
    @assert(total == 736)
}
