func append(v: ...)
    #validifx
    {
        @assert(@countof(v) == 3)
        return true
    }
{
}

#test
{
    var x = [10, 20, 30]
    append(@spread(x))
}
