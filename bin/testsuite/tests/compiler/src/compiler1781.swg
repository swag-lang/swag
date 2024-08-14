#global testerror "Err0501"

#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    let x = [10]
    let y = 1
    let z = x[y]
}