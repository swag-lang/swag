#global testerror "Err0220"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}