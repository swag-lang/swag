//#global skip

#[Swag.Macro]
func parallelFor(count: u32, stmt: #code void)
{
    func visitFor()
    {
        #macro
        {
            #inject(#up stmt)
        }
    }
}

//struct A
struct(T) A
{
    count: u32
}

impl A
{
    #[Swag.Macro]
    mtd titi()
    {
        me.count = 0
    }
}

#test
{
    parallelFor(0)
    {
        //var a: A
        var a: A's32
        a.titi()
    }
}
