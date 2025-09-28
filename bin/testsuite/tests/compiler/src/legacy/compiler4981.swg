#global testwarning "Wrn0001"
#global #[Swag.Warning("Wrn0001", Swag.WarningLevel.Enable)]

#test
{
	@compilerwarning("toto $ titi $ tata", #curlocation)
}