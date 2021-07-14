//#[Swag.PrintBc]
#test
{
    a := 666
    if a == 666
    {
        #[Swag.mixin]
        func toto()
        {
            a = 1
        }

        toto()
    }

    @assert(a == 1)
}