struct A
{
    x = 666
}

var G = 0

impl A
{
    func opDrop(using self)
    {
        G += 1
    }
}

//#[Swag.PrintBc]
#test
{
    loop 4
    {
        if #index == 0 do
            continue // should not drop x !
        var x: A
        if #index == 2 do
            break
    }

    @assert(G == 2)
}