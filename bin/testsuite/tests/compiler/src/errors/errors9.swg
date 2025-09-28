#global testerror "Err0600"
#test
{
    // Data pointer → function pointer (also invalid)
    var v: u32 = 0
    let dp: [*] u8 = &v
    let fptr = cast(*func()->void) dp     // ERROR: Invalid cast
}
