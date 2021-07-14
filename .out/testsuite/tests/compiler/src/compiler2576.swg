//#[Swag.PrintBc]
struct AA
{
    ptr: *~64 s32
}

var G = 666

//#[Swag.PrintBc]
#test
{
    var x, y: AA
    y.ptr = &G
    @reloc(&x, &y)
    @assert(x.ptr[0] == 666)
}