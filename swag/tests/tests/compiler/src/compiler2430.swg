func append(buf: *void, fmt: string, values: ...)
    #selectif
    {
        if !@isconstexpr(fmt) return true
        return true
    }
{

}

#test
{
    append(null, "toto")
}