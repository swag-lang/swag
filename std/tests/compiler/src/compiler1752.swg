#module "compiler1752"
#unittest error

//#[swag.printbc]
#test
{
    x := "64"
    #[swag.safety(true)]
    y := &x[3]
}