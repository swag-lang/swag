struct AA
{
}


#[swag.inline]
func toto()->AA  
{ 
    return AA{}; 
}

//#[swag.printbc]
#test
{
    toto()
}