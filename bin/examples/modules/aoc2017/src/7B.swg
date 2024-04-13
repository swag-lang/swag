/*
--- Part Two ---
The programs explain the situation: they can't get down. Rather, they could get down, if they weren't expending all of their energy trying to keep the tower balanced. Apparently, one program has the wrong weight, and until it's fixed, they're stuck here.

For any program holding a disc, each program standing on that disc forms a sub-tower. Each of those sub-towers are supposed to be the same weight, or the disc itself isn't balanced. The weight of a tower is the sum of the weights of the programs in that tower.

In the example above, this means that for ugml's disc to be balanced, gyxo, ebii, and jptl must all have the same weight, and they do: 61.

However, for tknk to be balanced, each of the programs standing on its disc and all programs above it must each match. This means that the following sums must all be the same:

ugml + (gyxo + ebii + jptl) = 68 + (61 + 61 + 61) = 251
padx + (pbga + havc + qoyq) = 45 + (66 + 66 + 66) = 243
fwft + (ktlj + cntj + xhth) = 72 + (57 + 57 + 57) = 243
As you can see, tknk's disc is unbalanced: ugml's stack is heavier than the other two. Even though the nodes above ugml are balanced, ugml itself is too heavy: it needs to be 8 units lighter for its stack to weigh 243 and keep the towers balanced. If this change were made, its weight would be 60.

Given that exactly one program is the wrong weight, what would its weight need to be to balance the entire tower?
*/

using Core

func day7B() assume
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "7.txt")
    let lines    = assume File.readAllLines(dataPath)

    struct Pile
    {
        rank:       u64
        val:        s32
        childs:     Array'u64
    }

    var set: HashTable'(string, Pile)

    var r = Parser.RegExp.create(#"(\l+) \((\d+)\)"#)
    visit l: lines
    {
        discard r.partialMatch(l)
        var p: Pile{set.count, StrConv.toS32(r[2])}
        set.add(r[1], p)
    }

    var r1 = Parser.RegExp.create(#"\l+"#)
    visit l: lines
    {
        let n = r1.grepAll(l)
        if n.count == 1 do
            continue
        let me = set.find(n[0])
        visit v: n[1..] do
            me.value.childs.add(set.find(v).value.rank)
    }

    //Console.print(set)
}
