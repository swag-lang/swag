func append(v: ...)
    #selectifx
    {
        @assert(@countof(v) == 2)
        return true
    }
{
}

#test
{
    append(@spread([10, 20]))
}