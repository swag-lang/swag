#module "compiler1850"
#unittest error

struct A
{
    y: s32
    x: *u32
}

//#[swag.printbc]
#test
{
    var x : A
    #[swag.safety(true)]
    ptr := x.x[1]
}