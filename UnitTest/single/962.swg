#module module962

struct toto
{
	x: s32
	y: s32
}

//#[swag.printbc]
#test
{
	var t: [2] {rank: s32, value: toto}
	t1 := t[0]
	t1.rank = 666
	@assert(t1.rank == 666)
	
	//var x: {s32, s32, s32}
	//y := x
}