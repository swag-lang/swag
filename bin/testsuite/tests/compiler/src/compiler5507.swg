#global testerror "Err0156"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}