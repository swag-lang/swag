#global testwarning
#[Swag.Warn("wrn0001", Swag.WarnLevel.Enable)]
func toto()
{
    unreachable
    a := 0
    @print(a)
}