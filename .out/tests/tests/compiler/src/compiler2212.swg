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

#[swag.inline]
func swap(x, y: *A)
{
    tmp :=,moveraw :x
    :x =,nodrop,moveraw :y
    :y =,nodrop,moveraw tmp
}

//#[swag.printbc]
#test
{
    {
        var x, y: A
        swap(&x, &y)
    }

    @assert(G == 3)
}