//#[Swag.printbc]
#test
{
	var t: [2] {rank: s32, value: f64}
	t1 := t[0]
	t1.rank = 666
	@assert(t1.rank == 666)

	//var x: {s32, s32, s32}
	//y := x
}