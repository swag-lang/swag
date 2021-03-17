//
//#global testerror

func toto32(s: string, x: s32...)->s32
{
    @assert(@countof(x) == 3)
    @assert(@typeof(x).rawType == s32)
    return 0
}

#test
{
    toto32(s: "titi", x: @spread(@[1, 2, 4]))
}