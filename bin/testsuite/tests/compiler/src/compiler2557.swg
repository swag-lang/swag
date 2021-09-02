//#global testerror

func toto()
{
    #if false
        alias AA = s32
    #else
        alias AA = s64
    var x: AA
    @assert(@typeof(x) == s64)
}

#test
{
    toto()
}