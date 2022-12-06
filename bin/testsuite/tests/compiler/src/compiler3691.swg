alias F = #type func()

func toto()
{
}

func titi(x: F)
{
}

#test
{
    titi(@mkcallback(&toto))
}