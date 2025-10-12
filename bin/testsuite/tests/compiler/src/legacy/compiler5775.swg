#global skip
#global testerror "Saf0018"

#[Swag.Safety(.All, "", true)]
#[Swag.Optimize("", false)]
public func doModalLoop(cb: func(*void) = null)
{
    //if cb:
        cb(null)
}

#[Swag.Safety(.All, "", true)]
#[Swag.Optimize("", false)]
func test()
{
    doModalLoop(null)
}