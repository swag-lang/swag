#[Swag.discardable]
func itoa(result: *u8, value: s64)->*u8
{
    const conv = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"

    ptr := result
    orgValue := value
    loop
    {
        tmpValue := orgValue
        orgValue /= 10
        dref ptr = conv[35 + (tmpValue - (orgValue * 10))]
        ptr += 1
        if !orgValue
            break
    }

    // Negative value
    if value < 0
    {
        dref ptr = "-"'u8
        ptr += 1
    }

    // Returns the end address
    retVal := ptr

    // Zero terminated
    dref ptr = 0; ptr -= 1

    // Invert result
    ptr1 := result
    while ptr1 < ptr
    {
        tmpChar := dref ptr
        dref ptr  = dref ptr1; ptr -= 1
        dref ptr1 = tmpChar; ptr1 += 1
    }

    return retVal
}

func ftoa(result: *u8, value: f64)
{
    ipart := cast(s64) value
    fpart := value - cast(f64) ipart

    n := result
    if ipart == 0
    {
        if value < 0
        {
            dref n = "-"'u8
            n += 1
        }

        dref n = "0"'u8
        n += 1
    }
    else
        n = itoa(result, ipart);

    dref n = "."'u8; n += 1

    afterPoint := 5
    if fpart < 0
        fpart = -fpart
    while afterPoint
    {
        fpart *= 10
        afterPoint -= 1
    }

    ipart = cast(s64) fpart
    if fpart - ipart > 0.5 ipart += 1
    itoa(n, ipart)
}

#test
{
    var arr: [100] u8
    ftoa(&arr[0], -3.1415)
    var str: string = @mkstring(&arr[0], 7)
    //@print(str)
    @assert(str == "-3.1415")
}