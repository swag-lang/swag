#global skip
#global testerror "Saf0018"

#[Swag.Safety("", true)]
#[Swag.Optimize("", false)]
public func doModalLoop(cb: func(*void) = null)
{
    //if cb:
        cb(null)
}

#[Swag.Safety("", true)]
#[Swag.Optimize("", false)]
#test
{
    doModalLoop(null)
}