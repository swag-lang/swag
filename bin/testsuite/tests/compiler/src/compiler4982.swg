#global testwarning "Wrn0003"
#global #[Swag.Warning("Wrn0003", Swag.WarningLevel.Enable)]

#[Swag.Optimize(.All, false)]
#test
{
    var a = 0
    if a
    {
    }
}
