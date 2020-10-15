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
    func log(self)
    {
        @print(@kindof(self).name)
        @print("\n")
/*
        switch @kindof(self)
        {
        case login: @print("login\n")
        }*/
    }
}

#test
{
    var v: EventLogger

    v = EventLogger.login{"toto"}
    v.log()
    v = EventLogger.viewMedia{"toto", "titi"}
    v.log()
}