//#global testerror

//#[swag.printbc]
#test
{
    var x: [..]~4 s32
    v := @[10, 20]
    x = v
    @assert(@countof(x) == 2)
    @assert(x[0] == 10)
    @assert(x[1] == 20)

    y := x
    @assert(y[0] == 10)
    @assert(y[1] == 20)
}