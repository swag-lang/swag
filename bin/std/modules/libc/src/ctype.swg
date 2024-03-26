#global export
using Swag

#[Foreign(ForeignLib), Discardable]
{
    func isalnum(c: s32)->s32;
    func isalpha(c: s32)->s32;
    func isblank(c: s32)->s32;
    func iscntrl(c: s32)->s32;
    func isdigit(c: s32)->s32;
    func isgraph(c: s32)->s32;
    func islower(c: s32)->s32;
    func isprint(c: s32)->s32;
    func ispunct(c: s32)->s32;
    func isspace(c: s32)->s32;
    func isupper(c: s32)->s32;
    func isxdigi(c: s32)->s32;
    func tolower(c: s32)->s32;
    func toupper(c: s32)->s32;
}
