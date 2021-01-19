struct A
{
    v = 10
}

struct B
{
    v = 5
}

impl A
{
    func opEquals(using self, o: B)->bool
    {
        return v == o.v
    }
}

#test
{
    var x: A
    var y: B
    @assert(x != y)
    //@assert(y != x)
}