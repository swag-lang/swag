#global testerror "Saf0027"

#[Swag.Safety("", true)]
#[Swag.Optim("", false)]
#test
{
    const a = @acos(2.0'f32)
}