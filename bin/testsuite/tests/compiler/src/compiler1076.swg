

func toto() -> const [..] string
{
	return ["str0", "str1"]
}

#test
{
	var x = toto()[0]
	@assert(x == "str0")
	var y = toto()[1]
	@assert(y == "str1")
}