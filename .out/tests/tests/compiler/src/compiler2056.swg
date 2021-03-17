func swag_runtime_itoa(result: *u8, value: s64)->*u8
{
    const conv = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"

    ptr := result
    orgValue := value
    while true
    {
        tmpValue := orgValue
        orgValue /= 10
        :ptr = conv[35 + (tmpValue - (orgValue * 10))]
        ptr += 1
        if !orgValue
            break
    }

    // Negative value
    if value < 0
    {
        :ptr = "-"'u8
        ptr += 1
    }

    // Returns the end address
    retVal := ptr

    // Zero terminated
    :ptr = 0; ptr -= 1

    // Invert result
    ptr1 := result
    while ptr1 < ptr
    {
        tmpChar := :ptr
        :ptr    = :ptr1; ptr -= 1
        :ptr1   = tmpChar; ptr1 += 1
    }

    return retVal
}

#test
{
    var arr: [100] u8
    swag_runtime_itoa(&arr[0], -123456)
    var str: string = @mkstring(&arr[0], 7)
    @assert(str == "-123456")
}