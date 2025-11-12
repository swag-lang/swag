#test
{
    struct Point1 { x, y: s32 }
    struct Point2 { x, y: s32 }
    struct Point3 { x, y: s32 }
    struct Point4 { x, y: s32 }
    struct Point5 { x, y: s32 }
    struct Point6 { x, y: s32 }
    func set1(pt: *Point1, value: s32) { pt.x, pt.y = value }
    func set2(pt: *Point2, value: s32) { pt.x, pt.y = value }
    func set3(pt: *Point3, value: s32) { pt.x, pt.y = value }
    func set4(pt: *Point4, value: s32) { pt.x, pt.y = value }
    func set5(pt: *Point4, value: s32) { pt.x, pt.y = value }
    func set6(pt: *Point4, value: s32) { pt.x, pt.y = value }
}
