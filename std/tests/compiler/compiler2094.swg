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
    res := RR's32()
}
