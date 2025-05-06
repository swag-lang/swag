#global export
#foreignlib("iphlpapi")
using Swag

const MAX_ADAPTER_DESCRIPTION_LENGTH = 128
const MAX_ADAPTER_NAME_LENGTH        = 256
const MAX_ADAPTER_ADDRESS_LENGTH     = 8
const DEFAULT_MINIMUM_ENTITIES       = 32
const MAX_HOSTNAME_LEN               = 128
const MAX_DOMAIN_NAME_LEN            = 128
const MAX_SCOPE_ID_LEN               = 256
const MAX_DHCPV6_DUID_LENGTH         = 130
const MAX_DNS_SUFFIX_STRING_LENGTH   = 256

struct IP_ADDRESS_STRING
{
    str: [4 * 4] u8
}

alias IP_MASK_STRING = IP_ADDRESS_STRING
alias time_t         = u64

struct IP_ADDR_STRING
{
    next:          *IP_ADDR_STRING
    ipAddress:     IP_ADDRESS_STRING
    ipMask:        IP_MASK_STRING
    context:       DWORD
}

struct IP_ADAPTER_INFO
{
    next:                    *IP_ADAPTER_INFO
    comboIndex:              DWORD
    adapterName:             [MAX_ADAPTER_NAME_LENGTH + 4] u8
    description:             [MAX_ADAPTER_DESCRIPTION_LENGTH + 4] u8
    addressLength:           UINT
    address:                 [MAX_ADAPTER_ADDRESS_LENGTH] BYTE
    index:                   DWORD
    type:                    UINT
    dhcpEnabled:             UINT
    currentIpAddress:        *IP_ADDR_STRING
    ipAddressList:           IP_ADDR_STRING
    gatewayList:             IP_ADDR_STRING
    dhcpServer:              IP_ADDR_STRING
    haveWins:                BOOL
    primaryWinsServer:       IP_ADDR_STRING
    secondaryWinsServer:     IP_ADDR_STRING
    leaseObtained:           time_t
    leaseExpires:            time_t
}

#[Foreign("iphlpapi")]
{
    func GetAdaptersInfo(adapterInfo: *IP_ADAPTER_INFO, sizePointer: *ULONG)->ULONG;
}
