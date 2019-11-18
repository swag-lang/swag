//#[swag.printbc]
#test
{
    a := 666
    if a == 666
    {
        #[swag.macro]
        func toto()
        {
            a = 1
        }
        
        toto()
    }
    
    @assert(a == 1)
}