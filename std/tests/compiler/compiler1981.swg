struct(T) Array
{
}

struct String
{
}

func(T) titi(x: T)
{
    #assert(@typeof(T).kind == swag.TypeInfoKind.Struct)
}

impl Array
{
    func add(self, x: T)
    {
        #assert(@typeof(T).kind == swag.TypeInfoKind.Struct)
        titi(x)
    }
}

func getB()->String
{
    var x: String
    return x
}

#test
{
    var x: Array'String
    x.add(getB())
}