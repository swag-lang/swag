func variadic(x: ...)
{
    @assert(cast(s64) cast(s32) x[0] == 666)
}

func(T) isSorted(values: const [..] T)
{
    variadic(values[0])
}

#test
{
    var sl: [] s32 = @[666]
    isSorted(@mkslice(&sl[0], 1))
}
