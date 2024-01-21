#global testwarning "Wrn0001"
#global #[Swag.Warn("Wrn0001", Swag.WarnLevel.Enable)]

#test
{
	@compilerwarning("toto$titi$tata", #location)
}