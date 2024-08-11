#global testerror "Err0219"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}