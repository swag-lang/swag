#global testerror "Err0359"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}