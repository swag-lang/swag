#global testerror "Err0601"
#test
{
    // Integer literal → pointer
    let badPtr = cast([*] u8) 42        // ERROR: Invalid cast
}
