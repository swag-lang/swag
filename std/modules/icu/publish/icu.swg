namespace icu
{
    typealias UBool:    u8
    typealias UChar32:  char
    
    #[foreign("icudt")]
    func u_isWhitespace(UChar32 c)->UBool;
}