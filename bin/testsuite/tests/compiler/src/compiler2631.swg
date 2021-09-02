func toto()
{
    #if true struct AA {}
    var x: AA
}

#test
{
    toto()
}