//#[Swag.printbc]
#test
{
	args := @args()
	@assert(@countof(args) == 1) // exe path
	@assert(@typeof(args[0]) == string)
}