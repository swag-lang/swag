#global skip // no more relative pointers (18 july 2021)
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

//#[Swag.PrintBc, Swag.Optimize("bc",false)]
#test
{
    var a: *s32
    var x: *~32 s32 = a

    //var a, b: MNode
    //a.node[1].next = &b.node
    //@assert(a.node[1].next.value == 666)

    //var c = a
    //@print(cast(s64) c.next.value)
    //@assert(c.node[1].next.value == 666)
}