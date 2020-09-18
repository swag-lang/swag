#import "std"
using std

#test
{
	const str = "⺁⺆⻇"

	var c: char
	var ptr: const *u8 = @dataof(str)
	cpt := 0
	while true
	{
		(ptr, c) = std.utf8.nextChar(ptr)
		if !c
			break
		cpt += 1
	}

	@assert(@sizeof(str) != 3)
	@assert(cpt == 3)
}