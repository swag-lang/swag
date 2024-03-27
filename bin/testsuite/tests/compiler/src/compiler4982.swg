#global testwarning "Wrn0003"
#global #[Swag.Warn("Wrn0003", Swag.WarnLevel.Enable)]

#[Swag.Optim("", false)]
#test
{
    var a = 0
    if a
    {
    }
}
