func toto(x: ...) => 666

#test
{
    loop 10
    {
        var x = toto(1)
    }
}