struct AA
{
	x, y: s32
}

func toto(v: const* AA, zz: f32, yyyy: string, x: const [..] s32, a: const [2] f32, p: string...)
{
	@print(cast(f64) a[0])
	@print(cast(s64) x[0])
	@print(p[0])
}

func toto1(v: const* AA, zz: f32, yyyy: string, x: const [..] s32, a: const [2] f32, p: s32...)
{
	//@print(cast(f64) a[0])
	@print(cast(s64) x[0])
	@print(cast(s64) p[0])
}

#test
{
	var v: AA
	toto(&v, 3.14, "string", @[10, 20], @[1.1, 2.2], "toto", "titi")
	toto1(&v, 3.14, "string", @[10, 20], @[1.1, 2.2], 10, 20)
}