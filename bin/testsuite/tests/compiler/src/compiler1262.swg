//#[Swag.PrintBc]
func(T) toto()->T
{
    #if #typeof(T) == #typeof(string) do
        return "toto"
    #if #typeof(T) == #typeof(s64) do
        return 666
}

#test
{
    @assert(toto'(string)() == "toto")
    @assert(toto'(s64)() == 666)
}
