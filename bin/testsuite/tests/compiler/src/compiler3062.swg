#global testerror "Err0230"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}