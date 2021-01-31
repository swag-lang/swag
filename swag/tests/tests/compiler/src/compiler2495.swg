//#global testerror

struct Node
{
    #[swag.relative(4)]
    next: *Node

    value = 666
}

//#[swag.printbc]
#test
{
    var a, b: Node
    a.next = &b
    @assert(a.next.value == 666)

    c := a
    //@assert(c.next.value == 666)
    @memmove(&a, &b, @sizeof(a))
}