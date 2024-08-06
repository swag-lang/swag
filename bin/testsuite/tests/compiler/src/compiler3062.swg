#global testerror "Err0352"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}