#global skip
#global testerror "San0006"

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