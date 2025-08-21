struct AA
{

}

impl AA
{
    enum ENUM
    {

    }

    #[Swag.Macro]
    func titi(using me, ptr: *void, stmt: #code void)
    {
        func toto(ptr: *void)
        {
            #inject(stmt)
        }
    }

    func tata(using me)
    {
        titi(null)
        {
            var v = cast(*ENUM) ptr
        }
    }
}