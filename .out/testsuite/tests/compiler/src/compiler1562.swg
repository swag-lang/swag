struct(T) Array
{
}

struct BOUBOU
{
    value: s32 = 666
    value1: s32 = 666
}

impl BOUBOU
{
    func opDrop(using self)
    {
        @assert(false)
    }
}

//#[Swag.PrintBc]
#test
{
    var arr: Array'BOUBOU
}