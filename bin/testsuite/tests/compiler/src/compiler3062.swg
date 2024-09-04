#global testerror "Err0227"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}