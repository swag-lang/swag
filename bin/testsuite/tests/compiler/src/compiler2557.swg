//#global testerror

func toto()
{
    #if false:
        typealias AA = s32
    #else:
        typealias AA = s64
    var x: AA
    @assert(@typeof(x) == s64)
}

#test
{
    toto()
}