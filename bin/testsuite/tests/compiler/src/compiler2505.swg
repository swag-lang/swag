#global skip // no more relative pointers (18 july 2021)
//#global testerror

//#[Swag.PrintBc]
struct AA
{
    x: [2] *~32 s32
}

//#[Swag.PrintBc, Swag.Optimize("bc", false)]
#test
{
    var v = [10, 20]

    var s: AA
    s.x[0] = &v[0]
    s.x[1] = &v[1]

    var ss = s
    @assert(ss.x[0, 0] == 10)
    @assert(ss.x[0, 1] == 20)
}