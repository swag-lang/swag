/*struct(K) Array
{
}
*/

struct String1981
{
}

func(T) titi(x: const &T)
{
    //#print(#typeof(T).name)
    #assert(#typeof(T).kind == Swag.TypeInfoKind.Struct)
}

/*impl Array
{
    func add(self, x: K)
    {
        #assert(#typeof(K).kind == Swag.TypeInfoKind.Struct)
        titi(x)
    }
}*/

#test
{
    //var x: Array'String1981
    var b: String1981
    titi(b)
    //x.add(b)
}