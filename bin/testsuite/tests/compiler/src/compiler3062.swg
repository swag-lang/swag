#global testerror "Err0366"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}