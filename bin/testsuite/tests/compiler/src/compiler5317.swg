//#global testerror "Err0496"
//#[Swag.PrintBc]
#test
{
    const C = [1, 2, 3, 4]
    var v = 0
    loop i in 4
    {
        v += C[i]
    }
    //@print(v)
}