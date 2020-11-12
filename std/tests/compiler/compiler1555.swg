//using swag

struct(T) A
{
}

var G = 0

impl A
{
    func opDrop(using self)
    {
        G = 1
    }
}

//#[swag.printbc]
#test
{
    {
        G = 0
        var x: A's32
        @drop(&x)
        @assert(G == 1)
    }
}