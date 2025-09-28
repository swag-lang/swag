#global testerror "Err0601"

#test
{
    // char/codepoint → pointer
    let c: u32 = 'A'
    let p: [*] u8 = c                   // ERROR: Invalid cast
}
