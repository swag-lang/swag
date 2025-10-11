//#[Swag.PrintBc]
func average(x: any)
{
    let tt = @kindof(x)
    @assert(tt == #typeof(string))
}

//#[Swag.PrintBc]
#test
{
    average("0")
}
