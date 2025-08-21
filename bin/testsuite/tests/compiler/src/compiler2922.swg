struct AA {}
impl AA
{
    func titi(using me, x: s32 = 10)
    {
    }

    #[Swag.Inline]
    func toto(using me, x: s32)
    {
        titi()
    }

    func tata(using me)
    {
        toto(10)
    }
}