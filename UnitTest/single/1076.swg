#module module1076

func toto() -> [..] string
{
	return ["str0", "str1"]
}

#test
{
	@print(toto()[0])
}