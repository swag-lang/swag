#test
{
    let a  = 666
    var cb = func|a|()
    {
        var cb1 = func|a|()
        {
            a = 0
        }
    }
}
