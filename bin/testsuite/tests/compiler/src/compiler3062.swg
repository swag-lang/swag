#global testerror "Err0379"
struct AA {}

#test
{
    var a = AA{}
    var b = a[0 to 1]
}
