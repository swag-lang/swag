#global testwarning "Wrn0008"
#global #[Swag.Warning("Wrn0008", Swag.WarningLevel.Enable)]

#test
{
    var g: bool
    if g == false:
        g = true
    else:
        if g == true:
            g = false
}
