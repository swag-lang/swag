#global testerror "Err0015"

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
