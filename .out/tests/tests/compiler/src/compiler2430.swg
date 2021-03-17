public func append2430(buf: *void, fmt: string, values: ...)
    #checkif
    {
        if !@isconstexpr(fmt) return true
        return true
    }
{

}

#test
{
    append2430(null, "toto")
}