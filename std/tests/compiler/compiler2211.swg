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

//#[swag.printbc]
#test
{
    loop 4
    {
        if @index == 0
            continue // should not drop x !
        var x: A
        if @index == 2
            break
    }

    @assert(G == 2)
}