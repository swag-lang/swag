//#global testerror

//#[swag.printbc]
struct Node
{
    next: *~4 Node

    value = 666
}

//#[swag.printbc]
struct MNode
{
    value1 = 555
    node: [2] Node
}

//#[swag.printbc, swag.optim("bc",false)]
#test
{
    var a, b: MNode
    a.node[1].next = &b.node
    @assert(a.node[1].next.value == 666)

    c := a
    @assert(c.node[1].next.value == 666)
}