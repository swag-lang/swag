#module "compiler1324"
#unittest error

#[swag.mixin]
func toto()
{
    #macro
    {
        cpt := 0
    }
}

//#[swag.printbc]
#test
{
    cpt := 0
    toto()
}