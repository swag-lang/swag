//var G = 0

func append(buf: nl *void, fmt: string, values: ...)
    verify
    {
        if !#isconstexpr(fmt): return true
        var cpt = @countof(values)
        return true
    }
{
}

#run
{
    let x, y, z = 0
    append(null, "toto", x, y, z)
}