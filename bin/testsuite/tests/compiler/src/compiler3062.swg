#global testerror "Err0303"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}