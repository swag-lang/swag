#module "compiler1209"
#unittest error

public
#[swag.inline]
{
    func toto()
    {
        toto1()
    }
    
    func toto1()
    {
        //@print("toto\n")
    }    
}

//#[swag.printbc]
#test
{
    toto()
}