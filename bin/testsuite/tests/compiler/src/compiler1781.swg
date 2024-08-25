#global testerror "Err0512"

#[Swag.Optimize("", false)]
#[Swag.Safety("", true)]
#test
{
    let x = [10]
    let y = 1
    let z = x[y]
}