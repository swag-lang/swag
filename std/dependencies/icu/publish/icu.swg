namespace icu
{
    #foreignlib "icuuc"

    using swag

    typealias UBool:    u8
    typealias UChar32:  char
    
    #[foreign("icu")]
    {
        func u_isUWhiteSpace(c: UChar32)->UBool;
    }
}