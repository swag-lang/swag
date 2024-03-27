#global public
#global namespace G3239

struct SysCommandEvent3239
{
    enum Kind
    {
        Minimize
        Maximize
        Restore
        Close
    }

    sysKind: G3239.SysCommandEvent3239.Kind = undefined
}
