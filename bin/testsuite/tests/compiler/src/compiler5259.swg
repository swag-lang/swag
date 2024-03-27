#global testerror "Err0420"
struct A
{
}
impl A
{
}

#test
{
    var x: A
    visit x
    {
    }
}
