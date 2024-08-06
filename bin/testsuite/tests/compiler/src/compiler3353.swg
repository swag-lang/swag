#global testerror "Err0485"
enum A { R; B; }
#test
{
    var a = A.R
    #[Swag.Complete]
    switch a
    {
    case R: break
    }
}