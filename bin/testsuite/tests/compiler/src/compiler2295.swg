struct AA
{

}

impl AA
{
    func opAffect(using me, v: string)
    {
    }
}

#test
{
    var x: AA
    var ptr = &x
    dref ptr = "toto"
}