#import "std"
using std

#test
{
	const str = "⺁⺆⻇"

	ptr := @dataof(str)
    count := @countof(str)
	cpt := 0
	while true
	{
        slice := @mkslice(ptr, count)
		(c, eat) := std.utf8.firstChar(slice)
		if !eat
			break
        ptr += eat
        count -= eat
		cpt += 1
	}

	@assert(@sizeof(str) != 3)
	@assert(cpt == 3)
}