//#global skip
using Core

struct Node
{
    name:     String
    kids:     Array'*Node
}

func release(node: nn *Node)
{
    foreach kid in node.kids:
        kid.release()
    Memory.delete(node)
}

alias WalkAction = func||(const *Node, s32)

func walk(tree: nn const *Node, action: WalkAction, depth = 0)
{
    action(tree, depth)
    foreach kid in tree.kids
    {
        walk(kid, action, depth + 1)
    }
}

func print(tree: nn const *Node)
{
    walk(tree, func(node, depth)
    {
        //Console.printf("% %\n", 2 * depth, dref node)
    })
}

func calcTotalDepth(tree: nn *Node)->s32
{
    var total = 0
    walk(tree, func|&total|(node, depth)
    {
        total += depth
    })

    return total
}

#[Swag.Discardable]
func process(intro: nn *Node)->*Node
{
    let tree = Memory.new'Node()
    tree.name = "root"
    intro.kids.add(tree)

    let internalIntro = #ref intro.kids[0]
    internalIntro.print()
    tree.print()

    var totalDepth = 0
    for i in 200_000:
        totalDepth += intro.calcTotalDepth()

    //Console.printf("Total depth: %", totalDepth)
    return internalIntro
}

#test
{
    let intro = Memory.new'Node()
    defer intro.release()
    process(intro)
}
