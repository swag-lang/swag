public enum SurfaceMessageKind
{
    None
    Resize
    Paint
}

public struct SurfaceMessage
{
    kind:   SurfaceMessageKind = .None
    param0: u64
    param1: u64
}