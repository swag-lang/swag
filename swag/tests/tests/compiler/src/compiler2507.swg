//#global testerror

//#[swag.printbc, swag.optim("bc", false)]
#test
{
    var x: [..] s32
    v := @[10, 20]
    x = v
    @assert(@countof(x) == 2)
    //@assert(x[0] == 10)
}