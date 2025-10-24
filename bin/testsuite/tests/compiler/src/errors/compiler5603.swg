#global testwarning "Wrn0008"
#global #[Swag.Warning("Wrn0008", Swag.WarningLevel.Enable)]

#test
{
    var g: bool
    if g == false do
        g = true
    else do
        if g == true do
            g = false
}
