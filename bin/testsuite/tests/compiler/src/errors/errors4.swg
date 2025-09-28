#global testerror "Err0530"

#test
{
    // bool → pointer
    let b: bool = true
    let p: [*] u8 = b                   // ERROR: Invalid cast
}
