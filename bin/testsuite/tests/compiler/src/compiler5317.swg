//#global testerror "Err0547"
//#[Swag.PrintBc]
#test
{
    const C = [1, 2, 3, 4]
    var v = 0
    loop i: 4
    {
        v += C[i]
    }
    //@print(v)
}