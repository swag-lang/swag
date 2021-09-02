struct AA
{

}

impl AA
{
    func opAffect(using self, v: string)
    {
    }
}

#test
{
    var x: AA
    var ptr = &x
    ptr[0] = "toto"
}