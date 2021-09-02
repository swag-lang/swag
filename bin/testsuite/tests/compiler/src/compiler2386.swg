#global testerror

func toto()->const& s32
{
    return 0
}

#test
{
    v := 2
    var v1: const &s32
    v1 = v
    @print(@typeof(v1).name)
}