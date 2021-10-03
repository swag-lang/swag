using Core, CommandLine

#test
{
    struct TT
    {
        value:  s8
    }

    var x: TT
    a0 := try parse(&x, @["-value:1"])
    @assert(x.value == 1)
    a1 := try parse(&x, @["-value=10"])
    @assert(x.value == 10)
    a2 := try parse(&x, @["-value= 20"])
    @assert(x.value == 20)
}