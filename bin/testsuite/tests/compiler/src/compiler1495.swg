namespace std.os
{
    using std.diagnostics.Debug
    const A = 0
}

namespace std.diagnostics.Debug.os
{
    const A = 1
}

#assert std.os.A == 0
