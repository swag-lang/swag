#global testerror "Err0259"
struct AA
{

}

#test
{
    var a = AA{}
    var b = a[0..1]
}