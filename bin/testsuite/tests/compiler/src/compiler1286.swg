//#[Swag.PrintBc]
#test
{
    a := 666
    if a == 666
    {
        #[Swag.Mixin]
        func toto()
        {
            a = 1
        }

        toto()
    }

    @assert(a == 1)
}