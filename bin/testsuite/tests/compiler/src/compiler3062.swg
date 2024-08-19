#global testerror "Err0232"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}