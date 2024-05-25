#global testwarning "Wrn0006"
#[Swag.Warning("Wrn0006", Swag.WarningLevel.Enable)]
func(T) toto(x: s32)
{
}

#test
{
    toto's32(0)
}