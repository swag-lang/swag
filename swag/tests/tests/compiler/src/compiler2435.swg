func append(v: ...)
    #selectif
    {
        @assert(@countof(v) == 3)
        return true
    }
{
}

#test
{
    x := @[10, 20, 30]
    append(@spread(x))
}