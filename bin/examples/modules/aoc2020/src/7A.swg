/*
--- Day 7: Handy Haversacks ---
You land at the regional airport in time for your next flight. In fact, it looks like you'll even have time to grab some food: all flights are currently delayed due to issues in luggage processing.

Due to recent aviation regulations, many rules (your puzzle input) are being enforced about bags and their contents; bags must be color-coded and must contain specific quantities of other color-coded bags. Apparently, nobody responsible for these regulations considered how long they would take to enforce!

For example, consider the following rules:

light red bags contain 1 bright white bag, 2 muted yellow bags.
dark orange bags contain 3 bright white bags, 4 muted yellow bags.
bright white bags contain 1 shiny gold bag.
muted yellow bags contain 2 shiny gold bags, 9 faded blue bags.
shiny gold bags contain 1 dark olive bag, 2 vibrant plum bags.
dark olive bags contain 3 faded blue bags, 4 dotted black bags.
vibrant plum bags contain 5 faded blue bags, 6 dotted black bags.
faded blue bags contain no other bags.
dotted black bags contain no other bags.
These rules specify the required contents for 9 bag types. In this example, every faded blue bag is empty, every vibrant plum bag contains 11 bags (5 faded blue and 6 dotted black), and so on.

You have a shiny gold bag. If you wanted to carry it in at least one other bag, how many different bag colors would be valid for the outermost bag? (In other words: how many colors can, eventually, contain at least one shiny gold bag?)

In the above rules, the following options would be available to you:

A bright white bag, which can hold your shiny gold bag directly.
A muted yellow bag, which can hold your shiny gold bag directly, plus some other bags.
A dark orange bag, which can hold bright white and muted yellow bags, either of which could then hold your shiny gold bag.
A light red bag, which can hold bright white and muted yellow bags, either of which could then hold your shiny gold bag.
So, in this example, the number of bag colors that can eventually contain at least one shiny gold bag is 4.

How many bag colors can eventually contain at least one shiny gold bag? (The list of rules is quite long; make sure you get all of it.)
*/

using Core

func day7A()
{
    day7A0()
    day7A1()
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day7A0()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "7.txt")
    let lines    = assume File.readAllLines(dataPath)

    var id: HashTable'(string, Array'string)

    foreach l in lines
    {
        foreach r in ["bags", "bag", "."] do
            l.replace(r, "")
        l.replace(" , ", ",")
        l.replace("  ", " ")
        l.trim()

        let split = Tokenize.split(l, " contain ")
        if split[1] == "no other" do
            continue

        let split1 = Tokenize.split(split[1], ',''u8)
        foreach v in split1
        {
            let splitVal = Tokenize.split(v, ' ''u8, 1)
            foreach can in splitVal[1 to ]
            {
                let it = id.tryFind(split[0])
                if it do
                    it.value.add(can)
                else
                {
                    var tmp: Array'string
                    tmp.add(can)
                    id.add(split[0], tmp)
                }
            }
        }
    }

    func contains(id: *HashTable'(string, Array'string), what: string)->bool
    {
        let it = id.tryFind(what)
        if !it do
            return false

        foreach v in it.value
        {
            if v == "shiny gold" do
                return true
            if contains(id, v) do
                return true
        }

        return false
    }

    var total: u32
    foreach v in id do
        total += cast(s32) contains(&id, v)

    //Console.print("total: ", total)
    @assert(total == 296)
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
func day7A1()
{
    let dataPath = Path.combine(Path.getDirectoryName(#file), "7.txt")
    let lines    = assume File.readAllLines(dataPath)

    var id: HashTable'(string, Array'string)

    var r = assume Parser.RegExp.create(#"(\d+) (\w+ \w+)"#)
    foreach l in lines
    {
        let split = Tokenize.split(l, " bags contain ")
        foreach v in r.grepAll(split[1])
        {
            let splitVal = Tokenize.split(v, ' ''u8, 1)
            id.findOrAdd(split[0]).value.add(splitVal[1])
        }
    }

    func contains(id: *HashTable'(string, Array'string), what: string)->bool
    {
        let it = id.tryFind(what)
        if !it do
            return false

        foreach v in it.value
        {
            if v == "shiny gold" or contains(id, v) do
                return true
        }

        return false
    }

    var total: u32
    foreach v in id do
        total += cast(s32) contains(&id, v)

    //Console.print("total: ", total)
    @assert(total == 296)
}
