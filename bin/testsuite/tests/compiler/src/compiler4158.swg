#global testwarning "Wrn0006"
#[Swag.Warn("Wrn0006", Swag.WarnLevel.Enable)]
func(T) toto(x: s32)
{
}

#test
{
    toto's32(0)
}