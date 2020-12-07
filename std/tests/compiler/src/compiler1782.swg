#module "compiler1782"
#unittest error

//#[swag.printbc]
#[swag.safety(true)]
#test
{
    x := "toto"
    z := x[5]
}