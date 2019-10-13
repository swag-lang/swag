

#test
{
	args := @args()
	@assert(@countof(args) == 1) // exe path
	@assert(args[0] is string)
}