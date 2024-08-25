#global #if #os == Swag.TargetOs.Windows
using test_foreign

using Swag

#[Swag.Foreign("kernel32")]
{
    func GetModuleHandleA(lpModuleName: const *u8)->const *void;
    func GetProcAddress(hModule: const *void, lpProcName: const *u8)->const *void;
}

//#[Swag.PrintBc, Swag.Optimize(false)]
#test
{/*
    var h = GetModuleHandleA(@dataof("test_foreign"))
    @assert(h != null)
    var ptr = GetProcAddress(h, @dataof("test_foreign.tcf_manualcall"))
    @assert(ptr != null)

    var tcf_manualcall = cast(func(s64)->s64) ptr
    @assert(tcf_manualcall(10) == 10)*/
}
