#global testerror "Err0260"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}