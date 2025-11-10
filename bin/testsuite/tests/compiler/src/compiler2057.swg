#[Swag.Discardable]
func itoa(result: [*] u8, value: s64)->[*] u8
{
    const conv = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"

    var ptr      = result
    var orgValue = value
    for
    {
        let tmpValue = orgValue
        orgValue /= 10
        dref ptr = conv[35 + (tmpValue - (orgValue * 10))]
        ptr += 1
        if !orgValue do
            break
    }

    // Negative value
    if value < 0
    {
        dref ptr = '-''u8
        ptr += 1
    }

    // Returns the end address
    let retVal = ptr

    // Zero terminated
    dref ptr = 0; ptr -= 1

    // Invert result
    var ptr1 = result
    while ptr1 < ptr
    {
        let tmpChar = dref ptr
        dref ptr = dref ptr1; ptr -= 1
        dref ptr1 = tmpChar; ptr1 += 1
    }

    return retVal
}

func ftoa(result: [*] u8, value: f64)
{
    var ipart = cast(s64) value
    var fpart = value - cast(f64) ipart

    var n = result
    if ipart == 0
    {
        if value < 0
        {
            dref n = '-''u8
            n += 1
        }

        dref n = '0''u8
        n += 1
    }
    else do
        n = itoa(result, ipart)

    dref n = '.''u8; n += 1

    var afterPoint = 5
    if fpart < 0 do
        fpart = -fpart
    while afterPoint
    {
        fpart *= 10
        afterPoint -= 1
    }

    ipart = cast(s64) fpart
    if fpart - ipart > 0.5 do
        ipart += 1
    itoa(n, ipart)
}

#test
{
    var arr: [100] u8
    ftoa(&arr[0], -3.1415)
    let str: string = @mkstring(&arr[0], 7)
    //@print(str)
    @assert(str == "-3.1415")
}
