//#global skip
struct AA
{
}

interface BB
{
    x: func(self)->s32
}

var G = 0

impl BB for AA
{
    mtd x()->s32
    {
        G = 1
        return 555
    }
}

#test
{
    var x: AA
    ptr := &x
    itf := cast(BB) ptr
    @assert(itf.x() == 555)
    @assert(G == 1)
}