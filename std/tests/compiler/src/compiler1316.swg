#module "compiler1316"
#unittest error

//#[swag.printbc]
#test
{
    cpt := 0
    
    #inline
    {
        cpt = 10
    }
    
    @assert(cpt == 6)    
}