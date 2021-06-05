struct AA
{
	x, y: s32
}

func toto(v: const* AA, zz: f32, yyyy: string, x: const [..] s32, a: const [2] f32, p: string...)
{
    @assert(v.x == 1)
    @assert(v.y == 2)
	@assert(cast(f64) a[0] == 1.1)
	@assert(cast(s64) x[0] == 10)
	@assert(p[0] == "toto")
}

func toto1(v: const* AA, zz: f32, yyyy: string, x: const [..] s32, a: const [2] f32, p: s32...)
{
	//@assert(cast(f64) a[0] == 1.1)
	@assert(cast(s64) x[0] == 10)
	@assert(cast(s64) p[0] == 10)
}

func toto2(v1: AA)
{
	@assert(cast(s64) v1.x == 555)
}

#test
{
	var v: AA
    v.x = 1
    v.y = 2
	toto(&v, 3.14, "string", @[10, 20], @[1.1, 2.2], "toto", "titi")
	toto1(&v, 3.14, "string", @[10, 20], @[1.1, 2.2], 10, 20)
	v.x = 555
	v.y = 666
	toto2(v)
}