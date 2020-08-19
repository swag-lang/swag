#module "compiler1845"
#unittest error

//#[swag.printbc]
#test
{
    var ptr: func()
    #[swag.safety(true)]
    ptr()
}