//#global testerror

//#[Swag.PrintBc]
struct Node
{
    next: *~32 Node

    value = 666
}

//#[Swag.PrintBc]
struct MNode
{
    value1 = 555
    node: [2] Node
}

//#[Swag.PrintBc, Swag.Optim("bc",false)]
#test
{
    var a, b: MNode
    a.node[1].next = &b.node
    @assert(a.node[1].next.value == 666)

    c := a
    @assert(c.node[1].next.value == 666)
}