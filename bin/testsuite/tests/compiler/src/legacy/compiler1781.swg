#global testerror "Err0500"

#[Swag.Optimize("", false), Swag.Safety(.All, "", true)]
#test
{
    let x = [10]
    let y = 1
    let z = x[y]
}
