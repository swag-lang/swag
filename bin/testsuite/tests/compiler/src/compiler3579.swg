#test
{
    t := @typeof(#type closure())
    @assert(@countof(t.parameters) == 0)
    t1 := @typeof(#type closure(s32))
    @assert(@countof(t1.parameters) == 1)
}