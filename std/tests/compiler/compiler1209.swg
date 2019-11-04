#[swag.inline]
{
    func toto()
    {
        toto1()
    }
    
    func toto1()
    {
    }    
}

//#[swag.printbc]
#test
{
    toto()
}