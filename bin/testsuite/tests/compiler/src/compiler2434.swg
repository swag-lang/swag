func append(v: ...)
    where #call
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