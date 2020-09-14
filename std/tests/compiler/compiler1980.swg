struct A
{

}

func(T) titi(x: T)
{
    #assert(@typeof(T).kind == swag.TypeInfoKind.Struct)
}

func(T) toto(x: T)
{
    #assert(@typeof(T).kind == swag.TypeInfoKind.Struct)
    titi(x)
}

#test
{
    var x: A
    toto(x)
}