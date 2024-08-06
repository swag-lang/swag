#global testerror "Err0202"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}