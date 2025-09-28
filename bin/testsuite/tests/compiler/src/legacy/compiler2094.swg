struct(T) AA
{
    field: T
}

func(T) RR()->AA
{
    return AA'T{}
}

#test
{
    var res = RR's32()
}
