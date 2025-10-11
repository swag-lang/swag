/*
--- Day 9: All in a Single Night ---
Every year, Santa manages to deliver all of his presents in a single night.

This year, however, he has some new locations to foreach; his elves have provided him the distances between every pair of locations. He can start and end at any two (different) locations he wants, but he must foreach each location exactly once. What is the shortest distance he can travel to achieve this?

For example, given the following distances:

London to Dublin = 464
London to Belfast = 518
Dublin to Belfast = 141
The possible routes are therefore:

Dublin -> London -> Belfast = 982
London -> Dublin -> Belfast = 605
London -> Belfast -> Dublin = 659
Dublin -> Belfast -> London = 659
Belfast -> Dublin -> London = 605
Belfast -> London -> Dublin = 982
The shortest of these is London -> Dublin -> Belfast = 605, and so the answer is 605 in this example.

What is the distance of the shortest route?
*/

using Core

func day9A()
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
        if !town.contains(r[1]) do
            town.add(r[1], town.count)
        if !town.contains(r[2]) do
            town.add(r[2], town.count)
        dist.add(town.find(r[1]).value << 32 | town.find(r[2]).value, assume StrConv.toU64(r[3]))
        dist.add(town.find(r[1]).value | town.find(r[2]).value << 32, assume StrConv.toU64(r[3]))
    }

    for i in town.count do
        combi.add(i)

    var total = Swag.U64.Max
    for
    {
        var calc = 0'u64
        for i in cast(s64) combi.count - 1 do
            calc += dist.find(combi[i] << 32 | combi[i + 1]).value
        total = Math.min(total, calc)
        if !Slice.nextPermutation(combi.toSlice()) do
            break
    }

    //Console.print("total: ", total)
    @assert(total == 141)
}
