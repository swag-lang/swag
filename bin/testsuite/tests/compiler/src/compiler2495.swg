#global skip // no more relative pointers (18 july 2021)
//#global testerror

//#[Swag.PrintBc]
struct Node
{
    next: *~32 Node
    value = 666
}

//#[Swag.PrintBc]
#test
{
    var a, b: Node
    a.next = &b
    @assert(a.next.value == 666)

    c := a
    //@print(cast(s64) c.next.value)
    @assert(c.next.value == 666)
}