#global testwarning "wrn0002"
#[Swag.Warn("wrn0002", Swag.WarnLevel.Enable)]
func(T) toto(x: s32)
{
}

#test
{
    toto's32(0)
}