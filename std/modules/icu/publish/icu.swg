namespace icu
{
    #foreignlib "icudt"
    #foreignlib "icuuc"

    using swag

    typealias UBool:    u8
    typealias UChar32:  char
    
    #[foreign("icuuc")]
    {
        func u_isUWhiteSpace(c: UChar32)->UBool;
    }
}