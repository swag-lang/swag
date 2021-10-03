using Core, CommandLine

#test
{
    struct TT
    {
        value:  s8
        diff:   f32
        name:   string
    }

    var x: TT
    params := @["-value", "1", "-diff", "3.14"]
    a := parse(&x, params)

    @assert(a.isSet.value)
    @assert(a.isSet.diff)
    @assert(!a.isSet.name)
    @assert(x.value == 1)
    @assert(x.diff == 3.14)
}