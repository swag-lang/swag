#global export
#foreignlib("advapi32")
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

const KEY_QUERY_VALUE        = 0x0001
const KEY_SET_VALUE          = 0x0002
const KEY_CREATE_SUB_KEY     = 0x0004
const KEY_ENUMERATE_SUB_KEYS = 0x0008
const KEY_NOTIFY             = 0x0010
const KEY_CREATE_LINK        = 0x0020
const KEY_WOW64_32KEY        = 0x0200
const KEY_WOW64_64KEY        = 0x0100
const KEY_WOW64_RES          = 0x0300

const REG_NONE                       = 0'u32      // No value type
const REG_SZ                         = 1'u32      // Unicode nul terminated string
const REG_EXPAND_SZ                  = 2'u32      // Unicode nul terminated string
const REG_BINARY                     = 3'u32      // Free form binary
const REG_DWORD                      = 4'u32      // 32-bit number
const REG_DWORD_LITTLE_ENDIAN        = 4'u32      // 32-bit number (same as REG_DWORD)
const REG_DWORD_BIG_ENDIAN           = 5'u32      // 32-bit number
const REG_LINK                       = 6'u32      // Symbolic Link (unicode)
const REG_MULTI_SZ                   = 7'u32      // Multiple Unicode strings
const REG_RESOURCE_LIST              = 8'u32      // Resource list in the resource map
const REG_FULL_RESOURCE_DESCRIPTOR   = 9'u32      // Resource list in the hardware description
const REG_RESOURCE_REQUIREMENTS_LIST = 10'u32
const REG_QWORD                      = 11'u32     // 64-bit number
const REG_QWORD_LITTLE_ENDIAN        = 11'u32     // 64-bit number (same as REG_QWORD)

#[Foreign("advapi32"), Swag.Discardable]
{
    func RegCreateKeyExA(hKey: HKEY, lpSubKey: LPCSTR, reserved: DWORD, lpClass: #null LPSTR, dwOptions: DWORD, samDesired: REGSAM, lpSecurityAttributes: #null const *SECURITY_ATTRIBUTES, hkResult: *HKEY, lpdwDisposition: LPDWORD)->LSTATUS;
    func RegSetValueExA(hKey: HKEY, lpValueName: LPCSTR, reserved: DWORD, dwType: DWORD, lpData: #null const *BYTE, cbData: DWORD)->LSTATUS;
    func RegCloseKey(hKey: HKEY)->LSTATUS;
}
