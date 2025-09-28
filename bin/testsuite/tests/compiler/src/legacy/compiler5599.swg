#global testwarning "Wrn0007"
#global #[Swag.Warning("Wrn0007", Swag.WarningLevel.Enable)]

#test
{
    var a = 0
    switch a
    {
        case 0: 
            a = 1
            break
    }
}