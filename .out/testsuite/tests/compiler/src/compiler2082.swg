struct AA
{
}


#[Swag.inline]
func toto()->AA
{
    return AA{};
}

//#[Swag.printbc]
#test
{
    ex := toto()
}