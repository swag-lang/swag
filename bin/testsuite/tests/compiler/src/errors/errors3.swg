#global testerror "Err0600"
#test
{
    // Integer literal → pointer
    let badPtr = cast([*] u8) 42        // ERROR: Invalid cast
}
