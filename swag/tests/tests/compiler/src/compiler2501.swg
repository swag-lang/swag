//#global testerror

//#[swag.printbc]
struct Node
{
    value = 666

    #[swag.relative(4)]
    next: *s32
}

func toto()->Node
{
    var x: Node
    x.next = &x.value
    return x
}

//#[swag.printbc]
#test
{
    x := toto()
    @assert(x.next[0] == 666)
}