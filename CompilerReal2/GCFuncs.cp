
gcIncRefFunc := BoxFunc^
gcDecRefFunc := BoxFunc^
gcSetValueFunc := BoxFunc^
gcCallDestroy := BoxFunc^

gcClass := BoxClass^
gcArrayClass := BoxClassTemplate^

GCInit := !() -> void
{
	dummy := new Object ; $temp

	box := new FuncInputBox ; $temp
	box.itPars.Emplace(GTypeVoidP,false)

	gcIncRefFunc = FindFunc("internalGCIncRef",dummy,box^,false)
	if gcIncRefFunc == null
	{
		throw new Exception("Func internalGCIncRef not found")
	}
	gcIncRefFunc.ParseBlock()

	gcDecRefFunc = FindFunc("internalGCDecRef",dummy,box^,false)
	if gcDecRefFunc == null
	{
		throw new Exception("Func internalGCDecRef not found")
	}
	gcDecRefFunc.ParseBlock()

	gcCallDestroy = FindFunc("internalGCDestroyObj",dummy,box^,false)
 	if gcCallDestroy == null
 	{
 		throw new Exception("Func internalGCDestroyObj not found")
 	}
	gcCallDestroy.ParseBlock()

    
	box2 := new FuncInputBox ; $temp
	box2.itPars.Emplace(GTypeVoidP.GetPoint(),false)
	box2.itPars.Emplace(GTypeVoidP,false)
	gcSetValueFunc = FindFunc("internalGCSetPtr",dummy,box2^,false)
	if gcSetValueFunc == null
	{
		throw new Exception("Func internalGCSetPtr not found")
	}
	gcSetValueFunc.ParseBlock()
}

GCGetArrayPointer := !(Type^ typ) -> Type^
{
	if gcArrayClass == null
	{
		gcTmp := GetItem("TGCArray",null)
		gcArrayClass = gcTmp.Down->{BoxClassTemplate^}
	}
	itBox := new FuncInputBox ; $temp

	itBox.itConsts.Push(new ObjType(typ))

	return new TypeFatGCArr(gcArrayClass.GetClass(itBox^).GetPoint(),typ)
}

IsGCClass := !(Type^ toCmp) -> bool
{
	return toCmp is TypeClass and toCmp->{TypeClass^}.ToClass.IsGC
}
IsGCAnyPtr := !(Type^ toCmp) -> bool
{
	if toCmp is TypeFatGCArr
	{
		return true
	}
	return IsGCPtr(toCmp)
}
IsGCPtr := !(Type^ toCmp) -> bool
{
	return toCmp is TypePoint and toCmp.Base is TypeClass and toCmp.Base->{TypeClass^}.ToClass.IsGC
}

GCMakeAware := !(Object^ start, int allcId) -> void
{
	blk := GetBlock(start)
	if blk != null
	{
		blk.AddGC(allcId);
		AllocSetStruct(blk)
	}
}
TypeFatGCArr := class extend TypeFatArr
{
	realBase := Type^
	this := !(Type^ newBase, Type^ rBase) -> void
	{
		ItHash = newBase.ItHash*3 + 1
		Base = newBase
		realBase = rBase
		if DebugMode and Base.metaId != 0
		{
			metaId = GetNewId()
			itStr := "!"sbt + metaId + " = !DIDerivedType(tag:DW_TAG_pointer_type, baseType: !" + Base.metaId + " ,size: " + archSize + ", align: " + archSize + ")\n" 
			DebugMetaData.Push(itStr.Str())
		}
	}
	GetType := virtual !() -> string
	{
		return "GCfatarr"
	}
	GetGoodName := virtual !() -> char^
	{
		if Base is TypeFatGCArr
		{
			return ""sbt + Base.GetGoodName() + "[]"
		}
		cls := Base.Base->{TypeClass^}.ToClass
		tp := cls.ItConsts[0]->{ObjType^}.MyType
		return ""sbt + tp.GetGoodName() + "[]"
	}
}
BuiltInTemplateSetGCFatArr := class extend BoxTemplate
{
	this := !() -> void
	{
		FuncName = "="
		OutputName = "error"

		emptType := Queue.{Type^}() ; $temp
		emptType.Push(null->{Type^})
		emptType.Push(null->{Type^})
		MyFuncType = GetFuncType(emptType,null->{bool^},null->{Type^},false,false)
	}
	GetPriority := virtual !(FuncInputBox itBox) -> int
	{
		pars := ref itBox.itPars
		if pars.Size() != 2 return 255
		if not pars[0].second return 255
		if not pars[0].first is TypeFatGCArr return 255
		if pars[1].first == GTypeVoidP return 1
		if not pars[0].first != pars[1].first return 0

		return 255
	}
	GetNewFunc := virtual  !(FuncInputBox itBox, TypeFunc^ funct) -> BoxFunc^
	{
		pars := ref itBox.itPars
		
		return new BuiltIn2SetGCPtr("=",funct)
	}
	DoTheWork := virtual !(int pri) -> void
	{
		
	}
}
BuiltIn2SetGCPtr := class extend BuiltIn2Func
{
	this := !(char^ fName, TypeFunc^ fType) -> void
	{
		FuncName = fName
		MyFuncType = fType
	}
	PrintFunc := virtual !(BuiltIn2Call^ trg,TIOStream f) -> void {
		id := trg.GenId()

		trg.Down.PrintPointPre(f)
		trg.Down.Right.PrintPre(f)
		
		f << "%Left" << id << " = bitcast "
		trg.Down.PrintPointUse(f)
		f << " to i8**\n"
		f << "%Right"<<id<<" = bitcast "
		trg.Down.Right.PrintUse(f) 
		f << " to i8*\n" 
		f << "call void@" << gcSetValueFunc.OutputName << "(i8** %Left"<<id<<", i8* %Right"<<id<<")"
		if DebugMode
		{
			dbgId := CreateDebugCall(trg)
			if dbgId != -1
			{
				f << ", !dbg !" << dbgId
			}
		}
		f << "\n"
	}
}
