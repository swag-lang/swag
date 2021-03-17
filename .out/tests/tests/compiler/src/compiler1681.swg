struct TT
{
    value:  s64 = 456
    value1: s64 = 456
    value2: s64 = 456
    value3: s64 = 456
    value4: s64 = 456
}

alias myStruct = TT

//#[swag.printbc]
func C(x: myStruct)
{
    //@print(cast(s64) x.value)
    @assert(x.value == 456)
}

#run
{
    var x: myStruct
    C(x)
}