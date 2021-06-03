public const BIG_ENDIAN = #gettag("Swag.Endian", string, "little") == "big"
public const WINDOWS = #os == "windows"
public const ASSERT = #cfg == "debug" or #cfg == "fast-debug" or #hastag("std.Core.assert")
