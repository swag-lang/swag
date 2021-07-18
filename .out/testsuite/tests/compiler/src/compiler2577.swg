#global skip // no more relative pointers (18 july 2021)
//#[Swag.PrintBc]
struct AA
{
    ptr: *~64 s32
}

var G = 666

//#[Swag.PrintBc]
#test
{
    var x, y: [2] AA
    y[1].ptr = &G
    @reloc(&x[0], &y[0], 2)
    @assert(x[1].ptr[0] == 666)
}