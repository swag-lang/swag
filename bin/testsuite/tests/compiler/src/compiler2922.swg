struct AA {}
impl AA
{
    func titi(me, x: s32 = 10) {}

    #[Swag.Inline]
    func toto(me, x: s32)
    {
        me.titi()
    }

    func tata(me)
    {
        me.toto(10)
    }
}
