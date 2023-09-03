#global testwarning "wrn0004"
#[Swag.Warn("wrn0004", Swag.WarnLevel.Enable)]
func(T) toto(x: s32)
{
}

#test
{
    toto's32(0)
}