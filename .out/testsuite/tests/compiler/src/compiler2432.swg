//var G = 0

func append(buf: *void, fmt: string, values: ...)
    #checkif
    {
        if !@isconstexpr(fmt) return true
        cpt := @countof(values)
        return true
    }
{
}

#run
{
    x, y, z := 0
    append(null, "toto", x, y, z)
}