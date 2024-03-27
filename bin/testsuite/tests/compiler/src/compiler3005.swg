struct BB
{
}

var G = 0

impl BB
{
    func opDrop(using self)
    {
        G = 1
    }
}

struct AA
{
    x: BB
}

impl AA
{
    func opDrop(using self)
    {
        @assert(G == 0)
    }
}

#test
{
    {
        var v: AA
    }

    @assert(G == 1)
}