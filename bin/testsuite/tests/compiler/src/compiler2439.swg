struct A
{
    v = 10
}

struct B
{
    v = 5
}

var G = 0

impl A
{
    func opEquals(using self, o: B)->bool
    {
        G += 1
        return v == o.v
    }
}

impl B
{
    func opEquals(using self, o: string)->bool
    {
        return false
    }
}

//#[Swag.PrintBc]
#test
{
    var x: A
    var y: B
    @assert(y != x)
    @assert(G == 1)
}