struct A
{
}

impl A
{
    func opEquals(using self, o: string)->bool
    {
        return o == "toto"
    }
}

#test
{
    var x: A
    //@assert("toto" == x)
}