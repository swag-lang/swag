//#[Swag.printbc]
func(T) toto()->T
{
	#if @typeof(T) == @typeof(string)
		return "toto"
	#if @typeof(T) == @typeof(s64)
		return 666
}

#test
{
	@assert(toto'(string)() == "toto")
	@assert(toto'(s64)() == 666)
}