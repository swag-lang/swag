#global testerror "Err0600"

#test
{
    // enum → pointer
    enum Color { Red, Green, Blue }
    let e: Color = Color.Green
    let p = cast([*] u8) e              // ERROR: Invalid cast
}
