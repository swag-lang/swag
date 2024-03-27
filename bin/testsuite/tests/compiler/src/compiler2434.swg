func append(v: ...)
    #validifx
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
