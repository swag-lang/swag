#module "compiler1849"
#unittest error

//#[swag.printbc]
#test
{
    var x : string
    #[swag.safety(true)]
    ptr := &x[1]
    @assert(:ptr == "A"'u8)
}