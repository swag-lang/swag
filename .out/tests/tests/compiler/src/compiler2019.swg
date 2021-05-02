#global skip
var arr: [4] s64 = @[10, 20, 30, 40]

func toto()->const& s64 => arr[0]

func tata(value: any)
{
    @assert(@kindof(value) == s64)
}

func titi(values: ...)
{
    tata(values[0])
}

#test
{
    titi(toto())
}