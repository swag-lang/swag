/*
--- Part Two ---
The sandstorm is upon you and you aren't any closer to escaping the wasteland. You had the camel follow the instructions, but you've barely left your starting position. It's going to take significantly more steps to escape!

What if the map isn't for people - what if the map is for ghosts? Are ghosts even bound by the laws of spacetime? Only one way to find out.

After examining the maps a bit longer, your attention is drawn to a curious fact: the number of nodes with names ending in A is equal to the number ending in Z! If you were a ghost, you'd probably just start at every node that ends with A and follow all of the paths at the same time until they all simultaneously end up at nodes that end with Z.

For example:

LR

11A = (11B, XXX)
11B = (XXX, 11Z)
11Z = (11B, XXX)
22A = (22B, XXX)
22B = (22C, 22C)
22C = (22Z, 22Z)
22Z = (22B, 22B)
XXX = (XXX, XXX)
Here, there are two starting nodes, 11A and 22A (because they both end with A). As you follow each left/right instruction, use that instruction to simultaneously navigate away from both nodes you're currently on. Repeat this process until all of the nodes you're currently on end with Z. (If only some of the nodes you're on end with Z, they act like any other node and you continue as normal.) In this example, you would proceed as follows:

Step 0: You are at 11A and 22A.
Step 1: You choose all of the left paths, leading you to 11B and 22B.
Step 2: You choose all of the right paths, leading you to 11Z and 22C.
Step 3: You choose all of the left paths, leading you to 11B and 22Z.
Step 4: You choose all of the right paths, leading you to 11Z and 22B.
Step 5: You choose all of the left paths, leading you to 11B and 22C.
Step 6: You choose all of the right paths, leading you to 11Z and 22Z.
So, in this example, you end up entirely on nodes that end in Z after 6 steps.

Simultaneously start on every node that ends with A. How many steps does it take before you're only on nodes that end with Z?
*/

using Core

// ********************************************
// For the record, i've cheated for this one !!
// ********************************************

func day8B()
{
    let dataPath = Path.combine(Path.getDirectoryName(#curlocation.fileName), "8.txt")
    let lines    = assume File.readAllLines(dataPath)

    var instructions: string
    var nodes:        Array'{ name: string, left, right: u64 }
    var nameToNode:   HashTable'(string, u64)

    var startNode: Array'u64

    foreach l, i in lines
    {
        if i == 0 do
            instructions = l.toString()
        elif i > 1
        {
            let name = l[0 to 2]
            nameToNode.add(name, i - 2)
            if name[2] == 'A' do
                startNode.add(i - 2)
        }
    }

    nodes.resize(lines.count - 2)
    foreach l, i in lines
    {
        if i <= 1 do
            continue
        let name   = l[0 to 2]
        let left   = l[7 to 9]
        let right  = l[12 to 14]
        let leftV  = nameToNode.find(left).value
        let rightV = nameToNode.find(right).value
        nodes[i - 2] = {name, leftV, rightV}
    }

    var curNode = startNode

    var onEnd: Array'u64
    onEnd.resize(startNode.count)

    // For each starting node, cound the length of the path to the first ending node
    // Count is stored in 'onEnd'
    for i in curNode
    {
        var instIndex: u64
        for
        {
            let inst = instructions[instIndex]
            instIndex = (instIndex + 1) % @countof(instructions)

            let c = curNode[i]
            if inst == 'L' do
                curNode[i] = nodes[c].left
            else do
                curNode[i] = nodes[c].right
            onEnd[i] += 1
            if nodes[curNode[i]].name[2] == 'Z' do
                break
        }
    }

    var total = onEnd[0]
    for i in 1 until onEnd.count do
        total = Math.lcm(cast(s64) total, cast(s64) onEnd[i])

    //Console.print("total: ", total)
    @assert(total == 16187743689077)
}
