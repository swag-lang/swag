struct AA {}

impl AA
{
    enum ENUM
    {
        Z
    }

    #[Swag.Macro]
    func titi(me, ptr: *void, stmt: #code void)
    {
        func toto(ptr: *void)
        {
            #inject(stmt)
        }
    }

    func tata(me)
    {
        me.titi(null)
        {
            var v = cast(*ENUM) ptr
        }
    }
}
