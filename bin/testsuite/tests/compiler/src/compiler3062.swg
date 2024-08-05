#global testerror "Err0365"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}