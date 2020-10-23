#module "compiler2139"
#unittest error

//#[swag.printbc]
#test
{
    x := @[1's64, 2, 3, 4]
    y := @spread(x)
}