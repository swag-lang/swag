struct(T: const [..] typeinfo) AA
{
    /*x: @typeof(T[0])
    y: @typeof(T[1])*/
    #assert T[0] == s32
    #assert T[1] == bool
}

#test
{
    var v: AA'(@[s32, bool])
    //@print(@typeof(v.x).name)
}