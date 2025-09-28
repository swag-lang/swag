#global testerror "Err0530"

#test
{
    // float → pointer
    let f: f32 = 3.14
    let p = cast([*] u8) f              // ERROR: Invalid cast
}
