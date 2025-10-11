/*
--- Part Two ---
The programs explain the situation: they can't get down. Rather, they could get down, if they weren't expending all of their energy trying to keep the tower balanced. Apparently, one program has the wrong weight, and until it's fixed, they're stuck here.

For any program holding a disc, each program standing on that disc forms a sub-tower. Each of those sub-towers are supposed to be the same weight, or the disc itself isn't balanced. The weight of a tower is the sum of the weights of the programs in that tower.

In the example above, this means that for ugml's disc to be balanced, gyxo, ebii, and jptl must all have the same weight, and they:: 61.

However, for tknk to be balanced, each of the programs standing on its disc and all programs above it must each match. This means that the following sums must all be the same:

ugml + (gyxo + ebii + jptl) = 68 + (61 + 61 + 61) = 251
padx + (pbga + havc + qoyq) = 45 + (66 + 66 + 66) = 243
fwft + (ktlj + cntj + xhth) = 72 + (57 + 57 + 57) = 243
As you can see, tknk's disc is unbalanced: ugml's stack is heavier than the other two. Even though the nodes above ugml are balanced, ugml itself is too heavy: it needs to be 8 units lighter for its stack to weigh 243 and keep the towers balanced. If this change were made, its weight would be 60.

Given that exactly one program is the wrong weight, what would its weight need to be to balance the entire tower?
*/

using Core

private struct Pile
{
    val:        s32
    childs:     Array'string
}

private var set: HashTable'(string, Pile)
private var result: s64

func day7B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "7.txt")
    let lines    = assume File.readAllLines(dataPath)

    var r0 = assume Parser.RegExp.create(#"(\l+) \((\d+)\)"#)
    foreach l in lines
    {
        discard r0.partialMatch(l)
        set.add(r0[1], Pile{assume StrConv.toS32(r0[2])})
    }

    var r1 = assume Parser.RegExp.create(#"\l+"#)
    foreach l in lines
    {
        let n = r1.grepAll(l)
        if n.count == 1 do
            continue
        let self = set.find(n[0])
        foreach v in n[1 to ] do
            self.value.childs.add(v)
    }

    func sum(w: Pile)->u64
    {
        if result do
            return 0

        var total: u64
        var sums:  Array'{ rank: u64, total: s64 }

        foreach v, i in w.childs
        {
            let s = sum(set.find(v).value)
            sums.add({i, s})
            total += s
        }

        if sums.count > 1 and result == 0
        {
            sums.sort(func(x, y) => x.total <=> y.total)
            if sums[sums.count - 2].total != sums[sums.count - 1].total
            {
                let v = set.find(w.childs[sums[sums.count - 1].rank])
                result = sums[sums.count - 2].total - sums[sums.count - 1].total + v.value.val
            }
        }

        return w.val + total
    }

    discard sum(set.find("eugwuhl").value)
    @assert(result == 420)
}
