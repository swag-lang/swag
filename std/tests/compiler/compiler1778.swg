#module compiler1778
//#unittest error

//#[swag.printbc]
#[swag.safety(true)]
#test
{
    x := [1, 2]
    @memcpy(&x[0], &x[0], 0)
    //@memcpy(null, null, 0)
}