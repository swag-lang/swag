#global testerror "Err0155"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}