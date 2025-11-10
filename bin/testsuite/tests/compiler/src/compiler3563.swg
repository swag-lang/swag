struct Arr
{
    a:   CB
}

impl Arr
{
    mtd add(x: CB)
    {
        me.a = x
        me.a()
    }
}

var G = 0
func ref1()->CB
{
    let v = 2
    return func|v|()
    {
        G = v
    }
}

alias CB = func||()

#test
{
    var arr: Arr
    arr.add(ref1())
    @assert(G == 2)
}
