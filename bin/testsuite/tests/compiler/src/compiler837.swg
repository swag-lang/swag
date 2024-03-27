#global testwarning "Wrn0005"
#global #[Swag.Warn("Wrn0005", Swag.WarnLevel.Enable)]

#test
{
    loop 10
    {
        break
        var a = 0
    }
}
