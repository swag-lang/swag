public func append2430(buf: #null *void, fmt: string, values: ...)
    verify
    {
        if !#isconstexpr(fmt): return true
        return true
    }
{

}

#test
{
    append2430(null, "toto")
}