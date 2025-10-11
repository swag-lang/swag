func toto()
{
    #if true do
        struct AA {}
    var x: AA
}

#test
{
    toto()
}
