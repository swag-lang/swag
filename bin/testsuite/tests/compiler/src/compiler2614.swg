struct(T) AA
{

}

impl AA
{
    func toto() #validif @typeof(T) == s32
    {
    }
}

#test
{
    var x: AA's32
    AA's32.toto()
}