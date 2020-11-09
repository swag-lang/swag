struct(T) B
{
}

var G = 0

impl B
{
    func opIndexAffect(using self, index: u32, value: T)
    {
        G = 1
    }

    func(op: string) opIndexAssign(using self, index: u32, value: T)
    {
        G = 2
    }
}

#test
{
    var arr: B's32
    arr[0] = 0
    @assert(G == 1)
    arr[0] += 0
    @assert(G == 2)
}