#global testwarning "wrn0011"
#global #[Swag.Warn("wrn0011", Swag.WarnLevel.Enable)]

#test
{
	@compilerwarning("toto$titi$tata", #location)
}