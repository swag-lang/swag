#global testerror "Err0234"

#[Swag.Optimize(.All, false), Swag.Safety(.All, .All, true)]
#test
{
    let x = [10]
    let y = 1
    let z = x[y]
}
