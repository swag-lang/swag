func titi(line: string = #callerfunction)
{
	@assert(line == "compiler1128.tata")
}

func tata()
{
	titi()
}

#test
{
	tata()
}