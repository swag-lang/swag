#global testerror "Err0604"
const A = 2's16

func toto(x: s8) {}

#test
{
    toto(A)
}
