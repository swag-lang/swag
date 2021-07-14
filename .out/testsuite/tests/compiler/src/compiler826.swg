//#[Swag.PrintBc]
#test
{
	x := 256's64
	if x
		@assert(true)
	else
		@assert(false)

	x1 := 25's8
	if x1
		@assert(true)
	else
		@assert(false)

	x2 := 256's16
	if x2
		@assert(true)
	else
		@assert(false)

	x3 := 256'u16
	if x3
		@assert(true)
	else
		@assert(false)

	x4 := 256'u32
	if x4
		@assert(true)
	else
		@assert(false)

	x5 := 256's32
	if x5
		@assert(true)
	else
		@assert(false)

	x6 := 25'u8
	if x5
		@assert(true)
	else
		@assert(false)
}