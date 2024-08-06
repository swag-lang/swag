#global testerror "Err0308"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}