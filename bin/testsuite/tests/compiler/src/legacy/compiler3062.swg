#global testerror "Err0617"
struct AA {}

#test
{
    var a = AA{}
    var b = a[0 to 1]
}
