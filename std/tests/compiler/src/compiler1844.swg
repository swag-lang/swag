
#unittest error

//#[swag.printbc]
#test
{
    var ptr: *u8
    #[swag.safety(true)]
    ptr[0] = 0
}