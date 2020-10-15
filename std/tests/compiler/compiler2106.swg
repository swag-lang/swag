alias UserType = string
alias MediaType = s32

var G: s32

typeset EventLogger
{
    login{userType: UserType}
    logout{userType: UserType}
    viewMedia{fileName: string, userType: UserType, mediaType: MediaType}
}

impl typeset EventLogger
{
    func log(self)
    {
        //@print(@kindof(self).name)
        //@print("\n")

        switch @kindof(self)
        {
        case login: G += 1
        case viewMedia: G += 2
        }
    }
}

#test
{
    var v: EventLogger

    v = EventLogger.login{"toto"}
    v.log()
    v = EventLogger.viewMedia{"toto", "titi"}
    v.log()

    @assert(G == 3)
}