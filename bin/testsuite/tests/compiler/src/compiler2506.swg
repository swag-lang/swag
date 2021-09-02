#global skip // no more relative pointers (18 july 2021)
//#global testerror

//#[Swag.PrintBc, Swag.Optim("bc", false)]
#test
{
    var x: [..] *~32 s32
    var arr: [2] *~32 s32

    v := @[10, 20]
    arr[0] = &v[0]
    arr[1] = &v[1]

    x = arr
    @assert(x[0, 0] == 10)
    @assert(x[0, 1] == 20)
}