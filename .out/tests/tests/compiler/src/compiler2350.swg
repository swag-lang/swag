func(T) lambdaToCallback(lambda: func(T))
{
}

struct Point
{

}

func toto(t: Point)
{
}

#test
{
    lambdaToCallback(&toto)
}