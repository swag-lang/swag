#test
{
    let t = #typeof(func||())
    @assert(@countof(t.parameters) == 0)
    let t1 = #typeof(func||(s32))
    @assert(@countof(t1.parameters) == 1)
}
