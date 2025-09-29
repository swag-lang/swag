#global testerror "Err0600"

#test
{
    // bool → pointer
    let b: bool = true
    let p: [*] u8 = b                   // ERROR: Invalid cast
}
