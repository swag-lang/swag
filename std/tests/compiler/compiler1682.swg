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
func C(x: any)
{
    //@print(cast(s64) x.value)
    val := cast(myStruct) x
    @assert(val.value == 456)
}

#run
{
    var x: myStruct
    C(x)
}