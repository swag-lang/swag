func append(v: const [..] s32)
    #selectif
    {
        @assert(@countof(v) == 3)
        return true
    }
{
}

#test
{
    append(@[10, 10, 10])
    x := @[10, 10, 10]
    append(x)
}