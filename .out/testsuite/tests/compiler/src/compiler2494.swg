#global testerror

struct Node
{
    //#[Swag.relative(4)]
    next: *Node

    value = 666
}

//#[Swag.printbc]
#[Swag.safety("", true)]
#test
{
    var a, b: Node
    //a.next = &b
    @assert(a.next.value == 666)

    c := a
    //@assert(c.next.value == 666)
}