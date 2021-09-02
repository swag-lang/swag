struct AA
{
}


#[Swag.Inline]
func toto()->AA
{
    return AA{};
}

//#[Swag.PrintBc]
#test
{
    ex := toto()
}