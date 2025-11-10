//var G = 0

func append(buf: #null *void, fmt: string, values: ...)
    verify
    {
        if !#isconstexpr(fmt) do
            return true

        var cptPercent = 0
        for var i = 0; i < @countof(fmt); i += 1
        {
            if fmt[i] != '%''u8 do
                continue

            if i == @countof(fmt) - 1 or fmt[i + 1] != '%''u8 do
                cptPercent += 1
            else do
                i += 1
        }

        //@print(cast(s64) cptPercent)
        //@print("\n")
        return true
    }
{
}

#run
{
    append(null, "toto")
    append(null, "toto %")
    append(null, "% toto %")
    append(null, "% toto %%")
}
