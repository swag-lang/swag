//#global skip
using Core

struct Node
{
    name:     String
    kids:     Array'Node
}

alias WalkAction = closure(const &Node, s32)

func walk(tree: const &Node, action: WalkAction, depth = 0)
{
    action(tree, depth)
    foreach kid in tree.kids
    {
        walk(kid, action, depth + 1)
    }
}

func print(tree: const &Node)
{
    walk(tree, func(node, depth)
    {
        //Console.printf("% %\n", 2 * depth, node)
    })
}

func calcTotalDepth(tree: const &Node)->s32
{
    var total = 0
    walk(tree, closure|&total|(node, depth)
    {
        total += depth
    })

    return total
}

#[Swag.Discardable]
func process(intro: &Node)->&Node
{
    intro.kids.add({String.from("root")})

    let internalIntro = #ref intro.kids[0]
    internalIntro.print()
    intro.print()

    var totalDepth = 0
    for i in 200_000:
        totalDepth += intro.calcTotalDepth()

    //Console.printf("Total depth: %", totalDepth)
    return internalIntro
}

#test
{
    var intro: Node
    process(&intro)
}
