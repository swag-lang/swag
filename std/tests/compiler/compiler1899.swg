func toto(x = #callerlocation)
{
    //@error(x, "this is a test man !")
}

#test
{
    toto()
}