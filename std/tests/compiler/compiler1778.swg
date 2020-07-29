#module compiler1778
#unittest error

//#[swag.printbc]
#[swag.safety(true)]
#test
{
    @memcpy(null, null, 0)
}