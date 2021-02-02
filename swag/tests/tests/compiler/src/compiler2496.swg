//#global testerror

struct Node
{
    next: *~4 Node
    value = 666
}

//#[swag.printbc]
struct MNode
{
    value1 = 555
    node: Node
}

//#[swag.printbc]
#test
{
    var a, b: MNode
    a.node.next = &b.node
    @assert(a.node.next.value == 666)

    c := a
    //@print(cast(s64) c.next.value)
    @assert(c.node.next.value == 666)
}