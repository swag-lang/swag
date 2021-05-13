struct AA
{
    yy = false
}

impl AA
{
    func toto(using self, v: AA = @{true})
    {
    }
}

#test
{
    var xxx: AA
    xxx.toto()
}