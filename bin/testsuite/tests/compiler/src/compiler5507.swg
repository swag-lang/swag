#global testerror "Err0157"

struct A {}

func toto()->A
{
    return {}
}

#test
{
    let b = toto()[0]
}