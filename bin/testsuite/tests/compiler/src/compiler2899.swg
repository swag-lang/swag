#global skip
struct AA
{

}

impl AA
{
    func toto(const me)
    {
    }
}

#test
{
    var v: AA
    var r: const&AA = v
    r.toto()
}