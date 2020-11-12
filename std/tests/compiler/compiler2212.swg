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
    tmp := move nodrop :x
    :x = nodrop move nodrop :y
    :y = nodrop move nodrop tmp
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