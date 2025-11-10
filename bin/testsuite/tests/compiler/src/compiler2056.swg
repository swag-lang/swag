#[Swag.Discardable]
func swag_runtime_itoa(result: [*] u8, value: s64)->*u8
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

#test
{
    var arr: [100] u8
    swag_runtime_itoa(&arr[0], -123456)
    let str: string = @mkstring(&arr[0], 7)
    @assert(str == "-123456")
}
