impl PropertyList
{
    func refreshNative(item: *PropertyListItem)
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

    mtd validateNative(item: *PropertyListItem)
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

        hasChanged()
    }

    mtd constructNative(value: Swag.TypeValue, typeNative: const *Swag.TypeInfoNative, data: *u8)
    {
        let item = addItem(value, data)

        if Reflection.hasAttribute(value, EditSlider)
        {
            let slider = SliderCtrl.create(item.line, null)
            slider.labelSize = 0
            item.sliderBox   = slider

            let minV = cast(f32) Reflection.getAttributeValue(value, EditSlider, "min")
            let maxV = cast(f32) Reflection.getAttributeValue(value, EditSlider, "max")

            switch typeNative.nativeKind
            {
            case U8, U16, U32, U64:
                slider.setMinMaxU64(acast minV, acast maxV)
            case S8, S16, S32, S64:
                slider.setMinMaxS64(acast minV, acast maxV)
            case F32, F64:
                slider.setMinMaxF32(minV, maxV)
            }
        }
        else
        {
            let ed = EditBox.create(item.line, value.name)
            ed.maxSize.x   = maxSizeEditValue
            ed.anchorStyle = .X
            item.editBox   = ed

            switch typeNative.nativeKind
            {
            case U8:
                ed.setInputU64(Swag.U8.Min, Swag.U8.Max)
            case U16:
                ed.setInputU64(Swag.U16.Min, Swag.U16.Max)
            case U32:
                ed.setInputU64(Swag.U32.Min, Swag.U32.Max)
            case U64:
                ed.setInputU64(Swag.U64.Min, Swag.U64.Max)
            case S8:
                ed.setInputS64(Swag.S8.Min, Swag.S8.Max)
            case S16:
                ed.setInputS64(Swag.S16.Min, Swag.S16.Max)
            case S32:
                ed.setInputS64(Swag.S32.Min, Swag.S32.Max)
            case S64:
                ed.setInputS64(Swag.S64.Min, Swag.S64.Max)
            case F32, F64:
                ed.setInputF64()
            }
        }

        refreshNative(item)

        if item.sliderBox do
            item.sliderBox.sigChanged += closure|self, item|(ed) { self.validateNative(item); }
        elif item.editBox do
            item.editBox.sigLoseFocus += closure|self, item|(ed) { self.validateNative(item); }
    }
}
