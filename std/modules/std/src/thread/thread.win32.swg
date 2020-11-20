#if #os == "windows";
using win32

public impl Thread
{
    // Sleep the current thread for a given amount of milliseconds
    func sleep(ms: u32)
    {
        Sleep(ms)
    }

    #[swag.callback]
    private func threadEntry(param: *void)
    {
        th := cast(*Thread) param
        th.userLambda(:th)
    }

    // Creates a new thread
    func create(lambda: func(Thread), userParam: *void = null)->Thread
    {
        var result: retval
        result.userLambda = lambda
        result.userParam = userParam
        param := &result

        // As the threadEntry will be called by the os, need to be sure
        // we have a valid function pointer whatever the case is :
        // - we are running bytecode, and threadEntry should be run by the compiler
        // - we are native, and threadEntry is like a foreign function (native, i.e. callback)
        var paramBC: [2] *void
        var l64 = cast(u64) &threadEntry
        if l64 & swag.LambdaBcMarker
        {
            paramBC[0] = acast lambda
            paramBC[1] = acast &result
            param = acast &paramBC[0]
            l64 = acast @thrunptr()
        }
        else
        {
            l64 &= ~swag.LambdaForeignMarker
        }

        result.handle = CreateThread(null, 0, cast(*void) l64, cast(*void) param, CREATE_SUSPENDED, &result.id)
        return result
    }
}