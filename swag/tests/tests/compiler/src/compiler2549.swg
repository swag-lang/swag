func toto()
{
#if false throw "toto"
}

#test
{
    toto()
}
