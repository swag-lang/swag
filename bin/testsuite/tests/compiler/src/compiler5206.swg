#global skip
func toto() throw
{
    throw @err()
}

#test
{
    catch toto()
}