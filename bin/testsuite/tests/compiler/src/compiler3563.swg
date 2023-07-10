struct Arr
{
    a: CB
}

impl Arr
{
    mtd add(x: CB)
    {
        a = x
        a()
    }
}

var G = 0
func ref1()->CB
{
    let v = 2
    return closure|v|()
    {
        G = v
    }
}

alias CB = closure()

#test
{
    var arr: Arr
    arr.add(ref1())
    @assert(G == 2)
}