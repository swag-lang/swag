#test
{
    struct Point1 { x, y: s32; }
    struct Point2 { x, y: s32; }
    struct Point3 { x, y: s32; }
    struct Point4 { x, y: s32; }
    struct Point5 { x, y: s32; }
    struct Point6 { x, y: s32; }
    func set1(using pt: nn *Point1, value: s32) = x, y = value
    func set2(using pt: nn *Point2, value: s32) = x, y = value
    func set3(using pt: nn *Point3, value: s32) = x, y = value
    func set4(using pt: nn *Point4, value: s32) = x, y = value
    func set5(using pt: nn *Point4, value: s32) = x, y = value
    func set6(using pt: nn *Point4, value: s32) = x, y = value
}