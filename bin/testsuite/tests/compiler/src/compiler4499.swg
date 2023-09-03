#global testwarning "wrn0001"
#[Swag.Warn("wrn0001", Swag.WarnLevel.Enable)]
func toto()
{
    unreachable
    let a = 0
    @print(a)
}