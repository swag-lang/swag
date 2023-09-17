#global testerror "Saf0022"

#[Swag.Optim("", false)]
#[Swag.Safety("", true)]
#[Swag.Safety("sanity", true)]
#test
{
    var y = -10'f32
    y *= 2
    y -= 1
    var x = @sqrt(y)
}
