#global testwarning "wrn0009"
#global #[Swag.Warn("wrn0009", Swag.WarnLevel.Enable)]

#[Swag.Optim("", false)]
#test
{
	var a = 0
    if a {}
}