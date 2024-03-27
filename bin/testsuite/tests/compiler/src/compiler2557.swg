//#global testerror

func toto()
{
    #if false #do
        typealias AA = s32
    #else #do
        typealias AA = s64
    var x: AA
    @assert(@typeof(x) == s64)
}

#test
{
    toto()
}
