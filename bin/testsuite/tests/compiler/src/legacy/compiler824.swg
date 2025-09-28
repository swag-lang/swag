

//#[Swag.PrintBc]
#test
{
	var x = 256
	if x:
		@assert(true)
	else:
		@assert(false)

	x = 0
	if !x:
		@assert(true)
	else:
		@assert(false)
}