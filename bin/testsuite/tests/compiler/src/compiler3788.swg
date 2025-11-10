#[Swag.ConstExpr]
func countPercent(fmt: string)->u32
{
    var cptPercent = 0'u32
    for var i = 0; i < @countof(fmt); i += 1
    {
        if fmt[i] != '%''u8 do
            continue

        if i == @countof(fmt) - 1 or fmt[i + 1] != '%''u8 do
            cptPercent += 1
        else do
            i += 1
    }

    return cptPercent
}

#test
{
    var x = countPercent("%toto%  %")
}
