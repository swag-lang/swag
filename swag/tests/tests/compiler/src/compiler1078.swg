

func toto() -> [..] string
{
	return @["str0", "str1"]
}

#test
{
	@assert(toto()[0] == "str0")
	@assert(toto()[1] == "str1")
}