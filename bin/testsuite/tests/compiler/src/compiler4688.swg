#global testerror "Err0599"
enum A: const [2] any
{
    BB = null
}

//#global skip
//#[Swag.PrintBc]
#test
{
    const x = A.BB
    #assert x == null
}