#global testerror

#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    var x = [10]
    var y = 1
    var z = x[y]
}