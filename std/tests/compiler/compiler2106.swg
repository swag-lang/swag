alias UserType = string
alias MediaType = s32

typeset EventLogger
{
    login{userType: UserType}
    logout{userType: UserType}
    viewMedia{fileName: string, userType: UserType, mediaType: MediaType}
}

impl EventLogger
{
    func log(using self)
    {

    }
}

#test
{
    var v: EventLogger

    v = EventLogger.login{"toto"}
    //v.log()
}