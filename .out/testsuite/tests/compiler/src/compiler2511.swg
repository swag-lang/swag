#global skip // no more relative pointers (18 july 2021)
//#global testerror

//#[Swag.PrintBc]
#test
{
    v := @[10, 20]

    var x: [..]~32 s32 = v
    @assert(x[0] == 10)
    @assert(x[1] == 20)

    var y: [..] s32 = x
    @assert(y[0] == 10)
    @assert(y[1] == 20)
}