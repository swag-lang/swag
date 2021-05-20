public const BIG_ENDIAN = #tagval(string, "Swag.endian") == "big"
public const WINDOWS = #os == "windows"
public const ASSERT = #cfg == "debug" or #cfg == "fast-debug" or #hastag("std.Core.assert")
