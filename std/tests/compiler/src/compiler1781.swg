#module "compiler1781"
#unittest error

//#[swag.printbc]
#[swag.safety(true)]
#test
{
    x := [10]
    y := 1
    z := x[y]
}