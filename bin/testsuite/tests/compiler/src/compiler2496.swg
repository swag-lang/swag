#global skip // no more relative pointers (18 july 2021)
//#global testerror

struct Node
{
    next: *~32 Node
    value = 666
}

//#[Swag.PrintBc]
struct MNode
{
    value1 = 555
    node: Node
}

//#[Swag.PrintBc]
#test
{
    var a, b: MNode
    a.node.next = &b.node
    @assert(a.node.next.value == 666)

    var c = a
    //@print(cast(s64) c.next.value)
    @assert(c.node.next.value == 666)
}