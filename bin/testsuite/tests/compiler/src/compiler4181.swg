#global testerror "Saf0014"

#[Swag.Optimize("", false)]
#[Swag.Safety("", true)]
#[Swag.Sanity(true)]
#test
{
    var y = -10'f32
    y *= 2
    y -= 1
    var x = @sqrt(y)
}
