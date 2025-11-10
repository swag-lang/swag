var G = 0's64

struct AA {}

impl AA
{
    func opDrop(me)
    {
        G += 1
    }
}

#test
{
    {
        var x: AA
        let v  = 0
        switch v
        {
        case 0:
            break
        }
    }

    @assert(G == 1)
}
