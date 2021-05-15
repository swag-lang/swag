
using core

#test
{
	const str = "⺁⺆⻇"

	ptr := @dataof(str)
    count := @countof(str)
	cpt := 0
	loop
	{
        sl := @mkslice(ptr, cast(uint) count)
		(c, eat) := core.utf8.decodeRune(sl)
		if !eat
			break
        ptr += eat
        count -= eat
		cpt += 1
	}

	@assert(@sizeof(str) != 3)
	@assert(cpt == 3)
}