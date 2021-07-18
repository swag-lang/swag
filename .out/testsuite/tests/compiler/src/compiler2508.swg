#global skip // no more relative pointers (18 july 2021)
//#global testerror

//#[Swag.PrintBc]
#test
{
    var x: [..]~32 s32
    v := @[10, 20]
    x = v
    @assert(@countof(x) == 2)
    @assert(x[0] == 10)
    @assert(x[1] == 20)

    y := x
    @assert(y[0] == 10)
    @assert(y[1] == 20)
}