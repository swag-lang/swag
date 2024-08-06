#global testerror "Err0638"

#test
{
    //const a: any = 6
    //@assert(a == "6")

    var a: any = 6
    var b: any = 6
    @print(a == b)
}