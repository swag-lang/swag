//#[Swag.PrintBc]
func convertAny(value: any, strFormat: string)
{
    convertInt(cast(s64) value, strFormat)
}

func convertInt(value: s64, strFormat: string)
{
    @assert(value == 666)
    @assert(cast(string) strFormat == "toto")
}

#test
{
    var y: any = 666's64
    convertAny(y, "toto")
}