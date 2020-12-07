#module "compiler1780"
#unittest error

//#[swag.printbc]
#[swag.safety(true)]
#test
{
    x := 10
    x = x % 0
}