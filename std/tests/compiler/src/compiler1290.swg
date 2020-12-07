#module "compiler1290"
#unittest error

#[swag.mixin]
func toto()
{
    x := 666   
}

#test
{
    toto()
    x = 0
    toto()
}