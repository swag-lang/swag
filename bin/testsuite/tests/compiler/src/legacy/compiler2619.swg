#global testerror "Err0050"

#[Swag.NoCopy]
struct AA {}

#test
{
    let x: AA
    let y: AA = x
}
