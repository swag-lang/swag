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
            continue
        var x: A
        if @index == 2
            break
    }

    @print(cast(s64) G)
}