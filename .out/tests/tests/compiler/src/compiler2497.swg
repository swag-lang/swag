//#global testerror

//#[Swag.printbc]
struct Node
{
    next: *~32 Node
    value = 666
}

//#[Swag.printbc]
struct MNode
{
    value1 = 555
    node: [2] Node
}

//#[Swag.printbc, Swag.optim("bc",false)]
#test
{
    var a: *s32
    var x: *~32 s32 = a

    //var a, b: MNode
    //a.node[1].next = &b.node
    //@assert(a.node[1].next.value == 666)

    //c := a
    //@print(cast(s64) c.next.value)
    //@assert(c.node[1].next.value == 666)
}