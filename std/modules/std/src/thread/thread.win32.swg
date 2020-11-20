#if #os == "windows";
public namespace thread;
using win32

// Sleep the current thread for a given amount of milliseconds
func sleep(ms: u32)
{
    Sleep(ms)
}

#[swag.callback]
private func threadEntry(param: *void)
{
    th := cast(*Thread) param
    th.lambda()
}

func create(lambda: func(*void))->Thread
{
    var result: retval

    /*var paramBC: [2] *void
    //param := userParam

    var l64 = cast(u64) lambda
    if l64 & swag.LambdaBcMarker
    {
        paramBC[0] = acast lambda
        paramBC[1] = acast userParam
        param = acast &paramBC[0]
        l64 = acast @thrunptr()
    }
    else
    {
        l64 &= ~swag.LambdaForeignMarker
    }*/

    result.handle = CreateThread(null, 0, cast(*void) &threadEntry, cast(*void) &result, 0, &result.id)
    return result
}