#global export
#foreignlib "advapi32"
using Swag

alias ACCESS_MASK = DWORD
alias REGSAM      = ACCESS_MASK
alias HKEY        = const *void
alias LSTATUS     = LONG

const HKEY_CLASSES_ROOT                = cast(HKEY) cast(u64) 0x80000000
const HKEY_CURRENT_USER                = cast(HKEY) cast(u64) 0x80000001
const HKEY_LOCAL_MACHINE               = cast(HKEY) cast(u64) 0x80000002
const HKEY_USERS                       = cast(HKEY) cast(u64) 0x80000003
const HKEY_PERFORMANCE_DATA            = cast(HKEY) cast(u64) 0x80000004
const HKEY_PERFORMANCE_TEXT            = cast(HKEY) cast(u64) 0x80000050
const HKEY_PERFORMANCE_NLSTEXT         = cast(HKEY) cast(u64) 0x80000060
const HKEY_CURRENT_CONFIG              = cast(HKEY) cast(u64) 0x80000005
const HKEY_DYN_DATA                    = cast(HKEY) cast(u64) 0x80000006
const HKEY_CURRENT_USER_LOCAL_SETTINGS = cast(HKEY) cast(u64) 0x80000007

const REG_OPTION_NON_VOLATILE = 0x00000000

#[Foreign("advapi32")]
{
    func RegCreateKeyExA(hKey: HKEY, lpSubKey: LPCSTR, Reserved: DWORD, lpClass: LPSTR, dwOptions: DWORD, samDesired: REGSAM, lpSecurityAttributes: const *SECURITY_ATTRIBUTES, hkResult: *HKEY, lpdwDisposition: LPDWORD)->LSTATUS;
}
