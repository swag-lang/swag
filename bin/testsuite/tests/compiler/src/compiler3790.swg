namespace A
{
    func toto() {}
}

scopefile
{
    func toto() {}
}

#test
{
    using A//, B
    a := &toto
}