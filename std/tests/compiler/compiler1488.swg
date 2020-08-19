#module "compiler1488"
#unittest error

//#[swag.inline]
func toto(x: code)
{
}

//#[swag.printbc]
#test
{
    toto()
    {        
        @print("toto\n")
    }
}