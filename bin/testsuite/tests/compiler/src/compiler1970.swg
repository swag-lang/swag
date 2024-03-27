struct(T) AA
{
    x: s64
    y: T
}

func(T) toto()
{
    @assert(@sizeof(AA'T) == 16)
    @assert(@sizeof(T) == 8)
}

func(T) titi()
{
    toto'T()
}

#test
{
    titi's64()
}