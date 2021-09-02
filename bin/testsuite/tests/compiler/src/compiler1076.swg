

func toto() -> [..] string
{
	return @["str0", "str1"]
}

#test
{
	x := toto()[0]
	@assert(x == "str0")
	y := toto()[1]
	@assert(y == "str1")
}