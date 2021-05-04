/*//#global testerror
using swag

#[printbc]
struct Node
{
    value = 666
    next: *~32 s32
}

func toto()->Node
{
    var x: Node
    x.next = &x.value
    return x
}

//#[printbc]
#[optim("bc",false)]
#test
{
    x := toto()
    @assert(x.next[0] == 666)
}*/