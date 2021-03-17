struct AA
{

}

impl AA
{
    enum ENUM
    {

    }

    #[swag.macro]
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
            v := cast(*ENUM) ptr
        }
    }
}