#global if #os == "windows"
using tests_foreign

using swag

#[swag.foreign("kernel32")]
{
    func GetModuleHandleA(lpModuleName: const *u8)->const *void;
    func GetProcAddress(hModule: const *void, lpProcName: const *u8)->const *void;
}

//#[swag.printbc, swag.optim(false)]
#test
{
    h := GetModuleHandleA(@dataof("tests_foreign"))
    @assert(h != null)
    ptr := GetProcAddress(h, @dataof("tests_foreign.tcf_manualcall"))
    @assert(ptr != null)

    tcf_manualcall := @mkforeign(func(s64)->s64, ptr)
    @assert(tcf_manualcall(10) == 10)
}
