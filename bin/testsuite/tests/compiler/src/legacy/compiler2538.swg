#global testerror "Err0090"

#[Swag.NoCopy]
struct AA {}

#test
{
    let x: AA
    let y  = x
}
