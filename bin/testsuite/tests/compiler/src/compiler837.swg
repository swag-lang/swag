#global testwarning
#global #[Swag.Warn("wrn0001", Swag.WarnLevel.Enable)]

#test
{
	loop 10
	{
		break
		a := 0
	}
}