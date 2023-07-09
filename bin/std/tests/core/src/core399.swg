//#global skip
using Core

struct Node
{
    name:   String
    kids:   Array'Node
}

alias WalkAction = closure(const ref Node, s32)

func walk(tree: const ref Node, action: WalkAction, depth = 0)
{
    action(tree, depth)
    visit kid: tree.kids
    {
        walk(kid, action, depth + 1)
    }
}

func print(tree: const ref Node)
{
    walk(tree, func(node, depth) {
        //Console.printf("% %\n", 2 * depth, node)
    })
}

func calcTotalDepth(tree: const ref Node) -> s32
{
    var total = 0
    walk(tree, closure|ref total|(node, depth) {
        total += depth
    })

    return total
}

#[Swag.Discardable]
func process(intro: ref Node) -> ref Node
{
    intro.kids.add({String.from("root")})

    var internalIntro = ref intro.kids[0]
    internalIntro.print()
    intro.print()

    var totalDepth = 0
    loop i: 200_000
        totalDepth += intro.calcTotalDepth()

    //Console.printf("Total depth: %", totalDepth)
    return internalIntro
}

#test
{
    var intro: Node
    process(&intro)
}