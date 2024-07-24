func append(v: const [..] s32)
    where,call
    {
        @assert(@countof(v) == 3)
        return true
    }
{
}

#test
{
    append([10, 10, 10])
    var x = [10, 10, 10]
    append(x)
}