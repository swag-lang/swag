struct A
{

}

func(T) titi(x: T)
{
    @print(@typeof(T).name)
    @print("\n")
    @print(@typeof(x).name)
    @print("\n")
    //@assert(@typeof(x).kind == swag.TypeInfoKind.Struct)
}

func(T) toto(x: T)
{
    @print(@typeof(T).name)
    @print("\n")
    @print(@typeof(x).name)
    @print("\n")
    titi(x)
}

#test
{
    var x: A
    toto(x)
}