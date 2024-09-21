func append(v: ...)
    verify
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