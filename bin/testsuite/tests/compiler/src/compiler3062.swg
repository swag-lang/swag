#global testerror "Err0320"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}