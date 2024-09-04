//#global testerror "Err0549"
struct Node {x: s32;}
struct(T) Arr {}

impl Arr
{
    mtd(V) add(x: V)
    {
        @assert(x.item0 == 1)
    }
}

#test
{
    var arr: Arr'Node
    arr.add({1})
}