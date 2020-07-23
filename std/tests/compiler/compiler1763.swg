#[swag.printbc]
func toto()->string
{
    return "toto"
}

#[swag.printbc]
#test
{
    @print(toto())
}