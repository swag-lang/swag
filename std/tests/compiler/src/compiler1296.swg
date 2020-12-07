#module "compiler1296"
#unittest error

#[swag.mixin]
func toto(x: code)
{
    #inline
    {
        #mixin x
    }
}

#test
{
    toto()
}