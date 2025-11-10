struct A
{
    x    = 666
}

var G = 0

impl A
{
    func opDrop(me)
    {
        G += 1
    }
}

#[Swag.Inline]
func swap(x, y: *A)
{
    var tmp = #moveraw dref x
    dref x = #nodrop #moveraw dref y
    dref y = #nodrop #moveraw tmp
}

//#[Swag.PrintBc]
#test
{
    {
        var x, y: A
        swap(&x, &y)
    }

    @assert(G == 3)
}
