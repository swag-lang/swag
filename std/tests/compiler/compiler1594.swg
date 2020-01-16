struct(T) B
{
}

var G = 0

impl B
{
    func(op: string) opIndexAssign(using self, value: T, index: u32)
    {
        #if op == "="
            G = 1
        #else
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