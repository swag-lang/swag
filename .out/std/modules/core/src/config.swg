public const BIG_ENDIAN = #tagval(string, "swag.endian") == "big"
public const WINDOWS = #os == "windows"
public const ASSERT = #cfg == "debug" || #cfg == "fast-debug" || #hastag("std.core.assert")
