struct AA
{

}

impl AA
{
    enum ENUM
    {

    }

    #[Swag.Macro]
    func titi(using self, ptr: *void, stmt: code)
    {
        func toto(ptr: *void)
        {
            #mixin stmt
        }
    }

    func tata(using self)
    {
        titi(null)
        {
            var v = cast(*ENUM) ptr
        }
    }
}