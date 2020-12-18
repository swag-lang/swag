struct(K) Array
{
}

struct String1981
{
}

func(T) titi(x: T)
{
    //#print @typeof(T).name
    #assert(@typeof(T).kind == swag.TypeInfoKind.Struct)
}

impl Array
{
    func add(self, x: K)
    {
        #assert(@typeof(K).kind == swag.TypeInfoKind.Struct)
        titi(x)
    }
}

#test
{
    var x: Array'String1981
    var b: String1981
    x.add(b)
}