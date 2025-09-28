#global testwarning "Wrn0005"
#[Swag.Warning("Wrn0005", Swag.WarningLevel.Enable)]
func toto()
{
    unreachable
    let a = 0
    @print(a)
}