alias UserType = s32
alias MediaType = s32

typeset EventLogger
{
    login{userType: UserType}
    logout{userType: UserType}
    viewMedia{fileName: string, userType: UserType, mediaType: MediaType}
}

impl EventLogger
{

}