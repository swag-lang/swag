#global testerror
enum A { R; B; }
#test
{
    a := A.R
    #[Swag.Complete]
    switch a
    {
    case R: break
    }
}