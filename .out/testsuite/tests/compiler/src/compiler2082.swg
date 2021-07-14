struct AA
{
}


#[Swag.inline]
func toto()->AA
{
    return AA{};
}

//#[Swag.PrintBc]
#test
{
    ex := toto()
}