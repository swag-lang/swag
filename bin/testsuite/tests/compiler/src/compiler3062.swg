#global testerror "Err0375"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}