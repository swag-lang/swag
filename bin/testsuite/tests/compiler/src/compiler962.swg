//#[Swag.PrintBc]
#test
{
	var t: [2] {rank: s32, value: string}
	t[0].rank = 1
	t[1].rank = 10
	@assert(t[0].rank == 1)
	@assert(t[1].rank == 10)

	@assert(t[0].value == null)
	t[0].value = "null"
	@assert(t[0].value == "null")
}