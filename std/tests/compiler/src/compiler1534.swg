#module "compiler1534"
#unittest error

//#[swag.printbc]
#test
{
    x := cast(char) "A"
    @assert(x == "AA")
}
