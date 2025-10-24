#global testerror "Err0049"

#[Swag.NoCopy]
struct AA {}

#test
{
    let x: AA
    let y  = x
}
