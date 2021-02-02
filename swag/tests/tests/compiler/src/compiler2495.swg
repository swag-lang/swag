//#global testerror

//#[swag.printbc]
struct Node
{
    next: *~4 Node
    value = 666
}

//#[swag.printbc]
#test
{
    var a, b: Node
    a.next = &b
    @assert(a.next.value == 666)

    c := a
    //@print(cast(s64) c.next.value)
    @assert(c.next.value == 666)
}