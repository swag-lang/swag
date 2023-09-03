#global testwarning "wrn0001"
#global #[Swag.Warn("wrn0001", Swag.WarnLevel.Enable)]

#test
{
	loop 10
	{
		break
		var a = 0
	}
}