#test
{
    var t = @typeof(closure())
    @assert(@countof(t.parameters) == 0)
    var t1 = @typeof(closure(s32))
    @assert(@countof(t1.parameters) == 1)
}