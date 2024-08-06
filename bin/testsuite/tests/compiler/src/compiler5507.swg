#global testerror "Err0198"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}