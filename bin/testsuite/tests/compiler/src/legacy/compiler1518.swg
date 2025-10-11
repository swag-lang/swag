struct A
{
    myTruc:     u64
    buffer:     [2] u32
}

impl A
{
    #[Swag.Inline]
    func(T) titi(x, y: T) => 1'u32

    #[Swag.Macro]
    func(ptr: bool) toto(me, stmt: #code void)
    {
        #assert(ptr == true)
        for idx in titi's32(1, 1)
        {
            let shift: u32 = 0
            for 1
            {
                #macro
                {
                    var #alias0 = cast(bool) (#up me.buffer[#up idx] & #up shift)
                    #up me.myTruc = #up idx + 1
                    #inject(#up stmt)
                }
            }
        }
    }
}

//#[Swag.PrintBc]
#test
{
    var x: A
    x.toto'true()
    {
    }

    @assert(x.myTruc == 1)
}
