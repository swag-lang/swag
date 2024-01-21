#global testerror "Err0056"
#test
{
    //const R = 100
    //@init(R)
    const R = [1, 2, 3]
    @init(&R[0], 3)
}