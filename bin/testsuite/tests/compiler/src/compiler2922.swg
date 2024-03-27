struct AA {}
impl AA
{
    func titi(using self, x: s32 = 10)
    {
    }

    #[Swag.Inline]
    func toto(using self, x: s32)
    {
        titi()
    }

    func tata(using self)
    {
        toto(10)
    }
}