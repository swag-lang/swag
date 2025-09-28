#global testerror "Err0600"

#test
{
    var value: *u32
    // This should error: cannot cast u32 to pointer type
    let badPtr: [*] u8 = value  // ERROR: Invalid cast
}
