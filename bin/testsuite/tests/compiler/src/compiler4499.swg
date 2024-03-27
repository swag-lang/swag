#global testwarning "Wrn0005"
#[Swag.Warn("Wrn0005", Swag.WarnLevel.Enable)]
func toto()
{
    unreachable
    let a = 0
    @print(a)
}
