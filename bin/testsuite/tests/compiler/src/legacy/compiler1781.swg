#global testerror "Err0500"

#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, true)]
#test
{
    let x = [10]
    let y = 1
    let z = x[y]
}
