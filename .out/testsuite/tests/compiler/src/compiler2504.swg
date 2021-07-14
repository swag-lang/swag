//#global testerror

//#[Swag.PrintBc, Swag.optim("bc", false)]
#test
{
    var x: [2] *~8 s32
    #assert @sizeof(x) == 2

    v := @[10, 20]
    x[0] = &v[0]
    x[1] = &v[1]

    @assert(x[0, 0] == 10)
    @assert(x[1, 0] == 20)
}