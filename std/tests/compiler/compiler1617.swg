struct A
{

}

//#[swag.printbc]
impl A
{
    #[swag.macro]
    func(ptr: bool) opVisit(using self, stmt: code)
    {
    }

    func toto(using self)
    {
        x := 10'u32
        ptr := &x

        /*visit deref self
        {
        }*/
    }
}