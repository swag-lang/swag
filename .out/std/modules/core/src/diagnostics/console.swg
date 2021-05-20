#global public
#global namespace console

#[Swag.enumindex]
enum Color
{
    Black
    White
    Gray
    Red
    Blue
    Green
    Cyan
    Yellow
    Magenta
    DarkRed
    DarkBlue
    DarkGreen
    DarkCyan
    DarkYellow
    DarkMagenta
}

protected var g_silentMode:   s32
protected var g_lock:         sync.Mutex

// Use to access console with multiple threads.
// No console function is thread safe, so it is the user responsability
// to lock the console when necessary
func lock()   = g_lock.lock()
func unlock() = g_lock.unlock()

// Disable console output. This is a counter, so can be called
// multiple times with the same 'value'
// Like any other console functions, this is not thread-safe
// => call 'lock' before if necessary, and 'unlock' after
func silent(value: bool)
{
    debug.assert(value or g_silentMode > 0)
    g_silentMode += value ? 1 : -1
}

// Write a bunch of parameters to the console
func print(params: ...)
{
    var buf: ConcatBuffer
    visit v: params
        strconv.convertAny(&buf, v, null)
    printf(buf.toString())
}

// Write a line to the console
func println(params: ...)
{
    print(params)
    print("\n")
}
