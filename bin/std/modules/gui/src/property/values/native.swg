impl Properties
{
    func refreshNative(item: *PropertiesItem)
    {
        let typeNative = cast(const *Swag.TypeInfoNative) item.typeValue.pointedType
        let data       = item.data

        if item.sliderBox
        {
            #[Swag.Mixin]
            func(T) setEd()
            {
                let p = cast(*T) data
                item.sliderBox.slider.setValue(cast(f32) dref p)
            }

            item.sliderBox.invalidate()
            switch typeNative.nativeKind
            {
            case U8:
                setEd'u8()
            case U16:
                setEd'u16()
            case U32:
                setEd'u32()
            case U64:
                setEd'u64()
            case S8:
                setEd's8()
            case S16:
                setEd's16()
            case S32:
                setEd's32()
            case S64:
                setEd's64()
            case F32:
                setEd'f32()
            case F64:
                setEd'f64()
            }
        }
        elif item.editBox
        {
            #[Swag.Mixin]
            func(T) setEd()
            {
                let p = cast(*T) data
                item.editBox.setText(Format.toString("%", dref p))
            }

            item.editBox.invalidate()
            switch typeNative.nativeKind
            {
            case U8:
                setEd'u8()
            case U16:
                setEd'u16()
            case U32:
                setEd'u32()
            case U64:
                setEd'u64()
            case S8:
                setEd's8()
            case S16:
                setEd's16()
            case S32:
                setEd's32()
            case S64:
                setEd's64()
            case F32:
                setEd'f32()
            case F64:
                setEd'f64()
            }
        }
    }

    mtd validateNative(item: *PropertiesItem)
    {
        let typeNative = cast(const *Swag.TypeInfoNative) item.typeValue.pointedType
        let data       = item.data

        if item.sliderBox
        {
            #[Swag.Mixin]
            func(T) setEd()
            {
                var p = cast(*T) data
                dref p = cast(T) item.sliderBox.slider.getValue()
            }

            switch typeNative.nativeKind
            {
            case U8:
                setEd'u8()
            case U16:
                setEd'u16()
            case U32:
                setEd'u32()
            case U64:
                setEd'u64()
            case S8:
                setEd's8()
            case S16:
                setEd's16()
            case S32:
                setEd's32()
            case S64:
                setEd's64()
            case F32:
                setEd'f32()
            case F64:
                setEd'f64()
            }
        }
        elif item.editBox
        {
            #[Swag.Mixin]
            func(T) setEd()
            {
                var p = cast(*T) data
                dref p = catch StrConv.toNum'T(txt)
            }

            let txt = item.editBox.getText()
            switch typeNative.nativeKind
            {
            case U8:
                setEd'u8()
            case U16:
                setEd'u16()
            case U32:
                setEd'u32()
            case U64:
                setEd'u64()
            case S8:
                setEd's8()
            case S16:
                setEd's16()
            case S32:
                setEd's32()
            case S64:
                setEd's64()
            case F32:
                setEd'f32()
            case F64:
                setEd'f64()
            }
        }

        .hasChanged(item)
    }

    mtd constructNative(value: Swag.TypeValue, typeNative: const *Swag.TypeInfoNative, data: *u8, parentItem: *PropertiesItem)
    {
        let item = .addItem(value, data, parentItem)

        if Reflection.hasAttribute(value, .SliderMinMax)
        {
            with let slider = SliderCtrl.create(item.line, null)
            {
                .labelSize = 0
                .maxSize.x = cast() Reflection.getAttributeValue(value, me.EditSize, "size", me.sizeEditValue)
                if me.kind == .Grid do
                    .dockStyle = .CenterKeepSizeVert
                else do
                    .anchorStyle = .X
            }

            item.sliderBox = slider

            let minV = cast(f32) assume Reflection.getAttributeValue(value, .SliderMinMax, "min")
            let maxV = cast(f32) assume Reflection.getAttributeValue(value, .SliderMinMax, "max")

            switch typeNative.nativeKind
            {
            case U8, U16, U32, U64:
                slider.setMinMaxU64(cast() minV, cast() maxV)
            case S8, S16, S32, S64:
                slider.setMinMaxS64(cast() minV, cast() maxV)
            case F32, F64:
                slider.setMinMaxF32(minV, maxV)
            }
        }
        else
        {
            let unit = Reflection.getAttributeValue(value, .Unit, "value")

            with let ed = EditBox.create(item.line, value.name)
            {
                if unit != null do
                    .unit = cast(string) unit
                .maxSize.x = cast() Reflection.getAttributeValue(value, me.EditSize, "size", me.sizeEditValue)
                if me.kind == .Grid do
                    .dockStyle = .CenterKeepSizeVert
                else do
                    .anchorStyle = .X
            }

            var minV, maxV: s64
            var hasMinMax = false
            if Reflection.hasAttribute(value, .MinMax)
            {
                hasMinMax = true
                minV      = cast(s64) assume Reflection.getAttributeValue(value, .MinMax, "min")
                maxV      = cast(s64) assume Reflection.getAttributeValue(value, .MinMax, "max")
            }

            item.editBox = ed
            switch typeNative.nativeKind
            {
            case U8:
                if !hasMinMax do
                    ed.setInputU64(Swag.U8.Min, Swag.U8.Max)
                else do
                    ed.setInputU64(cast() minV, cast() maxV)
            case U16:
                if !hasMinMax do
                    ed.setInputU64(Swag.U16.Min, Swag.U16.Max)
                else do
                    ed.setInputU64(cast() minV, cast() maxV)
            case U32:
                if !hasMinMax do
                    ed.setInputU64(Swag.U32.Min, Swag.U32.Max)
                else do
                    ed.setInputU64(cast() minV, cast() maxV)
            case U64:
                if !hasMinMax do
                    ed.setInputU64(Swag.U64.Min, Swag.U64.Max)
                else do
                    ed.setInputU64(cast() minV, cast() maxV)

            case S8:
                if !hasMinMax do
                    ed.setInputS64(Swag.S8.Min, Swag.S8.Max)
                else do
                    ed.setInputS64(cast() minV, cast() maxV)
            case S16:
                if !hasMinMax do
                    ed.setInputS64(Swag.S16.Min, Swag.S16.Max)
                else do
                    ed.setInputS64(cast() minV, cast() maxV)
            case S32:
                if !hasMinMax do
                    ed.setInputS64(Swag.S32.Min, Swag.S32.Max)
                else do
                    ed.setInputS64(cast() minV, cast() maxV)
            case S64:
                if !hasMinMax do
                    ed.setInputS64(Swag.S64.Min, Swag.S64.Max)
                else do
                    ed.setInputS64(cast() minV, cast() maxV)

            case F32, F64:
                if !hasMinMax do
                    ed.setInputF64()
                else do
                    ed.setInputF64(cast() minV, cast() maxV)
            }
        }

        refreshNative(item)

        if item.sliderBox
        {
            item.sliderBox.sigChanged += mtd|item|(ed)
            {
                .validateNative(item)
            }
        }
        elif item.editBox
        {
            item.editBox.sigLoseFocus += mtd|item|(ed)
            {
                if ed.isValid() do
                    .validateNative(item)
                else do
                    ed.restoreText()
            }
        }
    }
}
