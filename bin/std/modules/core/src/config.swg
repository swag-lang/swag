public const BIG_ENDIAN = #gettag("Swag.Endian", string, "little") == "big"
public const WINDOWS = #os == Swag.TargetOs.Windows
public const ASSERT = #cfg == "debug" or #cfg == "fast-debug" or #hastag("Std.Core.Assert")
