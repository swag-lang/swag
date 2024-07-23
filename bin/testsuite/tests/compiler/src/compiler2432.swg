//var G = 0

func append(buf: *void, fmt: string, values: ...)
    where(each)
    {
        if !@isconstexpr(fmt) do return true
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