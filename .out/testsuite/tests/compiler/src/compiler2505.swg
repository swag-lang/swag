//#global testerror

//#[Swag.PrintBc]
struct AA
{
    x: [2] *~32 s32
}

//#[Swag.PrintBc, Swag.optim("bc", false)]
#test
{
    v := @[10, 20]

    var s: AA
    s.x[0] = &v[0]
    s.x[1] = &v[1]

    ss := s
    @assert(ss.x[0, 0] == 10)
    @assert(ss.x[0, 1] == 20)
}