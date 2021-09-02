struct A
{

}

func(T) titi(x: T)
{
    #assert(@typeof(T).kind == Swag.TypeInfoKind.Struct)
}

func(T) toto(x: T)
{
    #assert(@typeof(T).kind == Swag.TypeInfoKind.Struct)
    titi(x)
}

#test
{
    var x: A
    toto(x)
}