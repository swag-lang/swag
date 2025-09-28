struct AA
{

}

struct BB
{
    v1:     *CC
    using   base: AA
}

struct CC
{

}

func toto(x: AA)
{
}

func titi()
{
    var x: BB
    toto(x)
}