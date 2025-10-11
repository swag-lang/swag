//var G = 0

func append(buf: #null *void, fmt: string, values: ...)
    verify
    {
        if !#isconstexpr(fmt) do
            return true
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
