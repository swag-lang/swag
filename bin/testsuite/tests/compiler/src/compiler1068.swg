#global testerror "Err0566"

func toString() -> [..] u8
{
	return [0, 1, 2, 3]
}

#[Swag.Safety("boundcheck", true)]
#test
{
	let x = toString()
	@assert(x[4] == 3)
}