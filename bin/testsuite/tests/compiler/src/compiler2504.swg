#global skip // no more relative pointers (18 july 2021)
//#global testerror

//#[Swag.PrintBc, Swag.Optimize("bc", false)]
#test
{
    var x: [2] *~8 s32
    #assert(#sizeof(x) == 2)

    var v = [10, 20]
    x[0] = &v[0]
    x[1] = &v[1]

    @assert(x[0, 0] == 10)
    @assert(x[1, 0] == 20)
}