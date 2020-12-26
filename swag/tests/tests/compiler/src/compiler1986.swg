func(T) sort(values: [..] T)
{
    #assert @typeof(T) == s32
    @assert(@countof(values) == 4)
    @assert(values[0] == 1)
    @assert(values[3] == 10)
}

#test
{
    var x: [] s32 = @[1, 5, 2, 10]
    sort(x)
}