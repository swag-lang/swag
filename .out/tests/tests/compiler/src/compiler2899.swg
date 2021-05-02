struct AA
{

}

impl AA
{
    func toto(using self)
    {

    }
}

#test
{
    var v: AA
    var r: const&AA = v
    r.toto()
}