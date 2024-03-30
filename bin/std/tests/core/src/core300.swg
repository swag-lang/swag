using Core

#test
{
    struct TT
    {
        value: s8
    }

    var x: TT
    var a0 = try CommandLine.parse(&x, ["-value:1"])
    @assert(x.value == 1)
    var a1 = try CommandLine.parse(&x, ["-value=10"])
    @assert(x.value == 10)
    var a2 = try CommandLine.parse(&x, ["-value= 20"])
    @assert(x.value == 20)
}
