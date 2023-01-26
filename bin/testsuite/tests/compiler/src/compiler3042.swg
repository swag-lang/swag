struct(T: const [..] typeinfo) AA
{
    x: @mktype(T[0])
    y: @mktype(T[1])
    #assert T[0] == s32
    #assert T[1] == bool
}

#test
{
    var v: AA'([s32, bool])
    @assert(@typeof(v.x) == s32)
    @assert(@typeof(v.y) == bool)
}