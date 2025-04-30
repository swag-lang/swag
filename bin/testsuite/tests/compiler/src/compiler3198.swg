#global skip
#[Swag.Inline]
func toto()->s32
{
    #[Swag.Inline]
    func titi()->s32
    {
        #[Swag.Inline]
        func tata()->s32
        {
            return 666
        }

        return tata()
    }

    return titi()
}

//#[Swag.PrintBc]
#test
{
    let x = toto()
    @assert(x == 666)
}