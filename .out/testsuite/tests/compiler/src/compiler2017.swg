//#[Swag.printbc, Swag.optim("", false)]
func variadic(x: ...)
{
    //@print(cast(s64) cast(u8) x[0])
    @assert(cast(u8) x[0] == "t"'u8)
}

//#[Swag.printbc]
func isSorted(values: string)
{
    variadic(values[0])
}

#test
{
    isSorted("toto")
}
