struct(T: const [..] typeinfo) AA
{
    x: @typeof(T[0])
    y: @typeof(T[1])
}

#test
{
    var v: AA'(@[s32, bool])
    @print(@typeof(v.x).name)
}