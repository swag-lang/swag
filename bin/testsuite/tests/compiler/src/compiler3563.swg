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
func ref()->CB
{
    v := 2
    return @|v|()
    {
        G = v
    }
}

alias CB = closure()

#test
{
    var arr: Arr
    arr.add(ref())
    @assert(G == 2)
}