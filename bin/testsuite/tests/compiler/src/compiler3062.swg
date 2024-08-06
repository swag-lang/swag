#global testerror "Err0309"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}