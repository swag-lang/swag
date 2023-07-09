#global testerror

enum RGB { R; G; B; }
#test
{
    var x = RGB.R
    switch x
    {
    case R: break
    case RGB.R: break
    }
}
