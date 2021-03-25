struct AA
{

}

var G = 0
impl AA
{
    func toto()
    {
        G = 1
    }
}

#test
{
    var x: AA
    AA.toto()
}