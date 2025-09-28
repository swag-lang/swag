#global testerror "Err0601"

#test
{
    let value: u32 = 0x12345678
    // This should error: cannot cast u32 to pointer type
    let badPtr = cast([*] u8) value  // ERROR: Invalid cast
}

