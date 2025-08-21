struct A
{
    x: s32
}

impl A
{
    func toto(using me)
    {
        x = 0
    }

    func tata()
    {
        var pt: A
        pt.x = 0
    }
}
