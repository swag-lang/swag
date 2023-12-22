//#global skip
using Core

struct Node
{
    name:   String
    kids:   Array'*Node
}

func release(node: *Node)
{
    visit kid: node.kids do
        kid.release()
    Memory.delete(node)
}

typealias WalkAction = closure(const *Node, s32)

func walk(tree: const *Node, action: WalkAction, depth = 0)
{
    action(tree, depth)
    visit kid: tree.kids
    {
        walk(kid, action, depth + 1)
    }
}

func print(tree: const *Node)
{
    walk(tree, func(node, depth) {
        //Console.printf("% %\n", 2 * depth, dref node)
    })
}

func calcTotalDepth(tree: *Node) -> s32
{
    var total = 0
    walk(tree, closure|&total|(node, depth) {
        dref total += depth
    })

    return total
}

#[Swag.Discardable]
func process(intro: *Node)->*Node
{
    let tree = Memory.new'Node()
    tree.name = "root"
    intro.kids.add(tree)

    let internalIntro = ref intro.kids[0]
    internalIntro.print()
    tree.print()

    var totalDepth = 0
    loop i: 200_000 do
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