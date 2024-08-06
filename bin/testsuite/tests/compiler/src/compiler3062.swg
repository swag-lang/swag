#global testerror "Err0304"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}