public func append2430(buf: *void, fmt: string, values: ...)
    check
    {
        if !@isconstexpr(fmt) do return true
        return true
    }
{

}

#test
{
    append2430(null, "toto")
}