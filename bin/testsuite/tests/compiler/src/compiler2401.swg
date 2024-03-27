struct(T, N: u8) A
{
    x: [N] T
}

impl A
{
    func toto(using self)
    {
    }
}
#test
{
    var x: A'(s32, 5)
    x.toto()
}
