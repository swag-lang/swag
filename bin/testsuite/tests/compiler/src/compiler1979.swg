struct AA
{

}

func(T) titi(x: T)
{
    //@print(@typeof(x).name)
}

func toto(x: AA)
{
    //@print("!!!!!!!!!!!!!!!!!!!!!!!")
    //@print(@typeof(x).name)

    titi(x)
}

#test
{
    var v: AA
    toto(v)
}