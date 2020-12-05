struct AA
{
	x, y: s32
}

func toto(zz: const f32, yyyy: string, x: const [..] s32, a: const [2] f32, p: string...)
{
	@print(cast(f64) a[0])
	@print(cast(s64) x[0])
}

func toto1(v: const* AA, zz: f32, yyyy: string, x: const [..] s32, p: ...)
{
	@print(yyyy)
}

#test
{
	toto(3.14, "string", @[10, 20], @[1.1, 2.2], "toto", "titi")

	var v: AA
	toto1(&v, 3.14, "string", @[10, 20], "toto", 666)
}