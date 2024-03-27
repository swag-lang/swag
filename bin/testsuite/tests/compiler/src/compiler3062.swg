#global testerror "Err0374"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}