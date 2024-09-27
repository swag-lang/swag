func(T) one(x: T, print: nn func(T))
{
    print(x)
}

func fprint(x: string)
{
    @assert(x == "toto")
}

#test
{
    one("toto", &fprint)
}