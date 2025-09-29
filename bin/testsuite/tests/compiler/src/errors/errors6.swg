#global testerror "Err0600"

#test
{
    // char/codepoint → pointer
    let c: u32 = 'A'
    let p: [*] u8 = c                   // ERROR: Invalid cast
}
