/*
--- Day 8: Haunted Wasteland ---
You're still riding a camel across Desert Island when you spot a sandstorm quickly approaching. When you turn to warn the Elf, she disappears before your eyes! To be fair, she had just finished warning you about ghosts a few minutes ago.

One of the camel's pouches is labeled "maps" - sure enough, it's full of documents (your puzzle input) about how to navigate the desert. At least, you're pretty sure that's what they are; one of the documents contains a list of left/right instructions, and the rest of the documents seem to describe some kind of network of labeled nodes.

It seems like you're meant to use the left/right instructions to navigate the network. Perhaps if you have the camel follow the same instructions, you can escape the haunted wasteland!

After examining the maps for a bit, two nodes stick out: AAA and ZZZ. You feel like AAA is where you are now, and you have to follow the left/right instructions until you reach ZZZ.

This format defines each node of the network individually. For example:

RL

AAA = (BBB, CCC)
BBB = (DDD, EEE)
CCC = (ZZZ, GGG)
DDD = (DDD, DDD)
EEE = (EEE, EEE)
GGG = (GGG, GGG)
ZZZ = (ZZZ, ZZZ)
Starting with AAA, you need to look up the next element based on the next left/right instruction in your input. In this example, start with AAA and go right (R) by choosing the right element of AAA, CCC. Then, L means to choose the left element of CCC, ZZZ. By following the left/right instructions, you reach ZZZ in 2 steps.

Of course, you might not find ZZZ right away. If you run out of left/right instructions, repeat the whole sequence of instructions as necessary: RL really means RLRLRLRLRLRLRLRL... and so on. For example, here is a situation that takes 6 steps to reach ZZZ:

LLR

AAA = (BBB, BBB)
BBB = (AAA, ZZZ)
ZZZ = (ZZZ, ZZZ)
Starting at AAA, follow the left/right instructions. How many steps are required to reach ZZZ?
*/

using Core

func day8A()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "8.txt")
    let lines    = assume File.readAllLines(dataPath)

    var instructions: string
    var nodes:        Array'{ left, right: u64 }
    var nameToNode:   HashTable'(string, u64)

    var startNode, endNode: u64

    foreach l, i in lines
    {
        if i == 0 do
            instructions = l.toString()
        elif i > 1
        {
            let name = l[0 to 2]
            nameToNode.add(name, i - 2)
            if name == "AAA" do
                startNode = i - 2
            elif name == "ZZZ" do
                endNode = i - 2
        }
    }

    nodes.resize(lines.count - 2)
    foreach l, i in lines
    {
        if i <= 1 do
            continue
        let left   = l[7 to 9]
        let right  = l[12 to 14]
        let leftV  = nameToNode.find(left).value
        let rightV = nameToNode.find(right).value
        nodes[i - 2] = {leftV, rightV}
    }

    var total:   u64
    var curNode: u64 = startNode
    while curNode != endNode
    {
        foreach inst in instructions
        {
            if inst == 'L' do
                curNode = nodes[curNode].left
            else do
                curNode = nodes[curNode].right
            total += 1
            if curNode == endNode do
                break
        }
    }

    //Console.print("total: ", total)
    @assert(total == 14257)
}
