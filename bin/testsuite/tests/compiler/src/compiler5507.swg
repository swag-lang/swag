#global testerror "Err0175"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}