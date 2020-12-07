#module "compiler1848"
#unittest error

//#[swag.printbc]
#test
{
    var x: [..] s32
    #[swag.safety(true)]
    x[0] = 0
}