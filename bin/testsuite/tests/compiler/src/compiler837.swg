#global testwarning "Wrn0005"
#global #[Swag.Warning("Wrn0005", Swag.WarningLevel.Enable)]

#test
{
	loop 10
	{
		break
		var a = 0
	}
}