#global testerror "Err0535"

#test
{
    try toto()
}

func toto() {}
/*
func toto()
{
    titi()
#if false throw "toto"
}

func titi()
{
#if false throw "toto"
}*/