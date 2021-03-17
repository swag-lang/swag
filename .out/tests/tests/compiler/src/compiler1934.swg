var G = 0's64

struct AA
{

}

impl AA
{
    func opDrop(using self)
    {
        G += 1
    }
}

#test
{
    {
        var x: AA
        v := 0
        switch v
        {
            case 0:
                break
        }
    }

    @assert(G == 1)
}
