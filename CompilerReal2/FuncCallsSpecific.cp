
setTypeFuncsPtr := AVLMap.{Type^,BoxFunc^}
setTypeFuncsRef := AVLMap.{Type^,AVLMap.{Type^,BoxFunc^}}
SetTypeToClass := !(Type^ fromType, Type^ toType) -> BoxFunc^
{
	if fromType is TypePoint
	{
		if toType in setTypeFuncsPtr
			return setTypeFuncsPtr[toType]

		asCl := toType->{TypeClass^}.ToClass
		newFunc := new BuiltInFuncUno("SetType",GTypeVoidP,false,GTypeVoid,false,
		"%mdl## = bitcast i8* #1 to i8**\n"sbt +
		"%PreSet## = bitcast " + asCl.GetVTableTypeName() + "* " + asCl.GetVTableName() + " to i8*\n" +
		"store i8* %PreSet##, i8** %mdl##\n"
		)
		setTypeFuncsPtr[toType] = newFunc
		return newFunc
	}else{
		assert(fromType is TypeClass)
		
		if fromType in setTypeFuncsRef
		{
			if toType in setTypeFuncsRef[fromType]
				return setTypeFuncsRef[fromType][toType]
		}

		asCl := toType->{TypeClass^}.ToClass
		newFunc := new BuiltInFuncUno("SetType",fromType,true,GTypeVoid,false,
		"%mdl## = bitcast "sbt + fromType.GetName() + "* #1 to i8**\n" +
		"%PreSet## = bitcast " + asCl.GetVTableTypeName() + "* " + asCl.GetVTableName() + " to i8*\n" +
		"store i8* %PreSet##, i8** %mdl##\n"
		)
		setTypeFuncsRef[fromType][toType] = newFunc
		return newFunc
	}
}

BuiltIn2FuncGetVTable := class extend BuiltIn2Func
{
	this := !(char^ fName, TypeFunc^ fType) -> void
	{
		FuncName = fName
		MyFuncType = fType
	}
	PrintFunc := virtual !(BuiltIn2Call^ trg,TIOStream f) -> void 
	{
		id := trg.GenId()
	
		cl := MyFuncType.Pars[0]->{TypeClass^}.ToClass

		trg.PrintRes(f)
		f << " = bitcast "
		cl.PrintVTableTypeName(f)
		f << "* "
		cl.PrintVTableObject(f)
		f << " to i8*\n"
	}
}

FuncCallSpecific := !(Object^ iter) -> Object^
{
	if iter.Down?.Right?.GetValue() == "->"
	{
		if iter.Down is ObjType
		{
			asTyp := iter.Down->{ObjType^}.MyType
			if iter.Down.Right.Right? is ObjIndent
			{
				asObj := iter.Down.Right.Right->{ObjIndent^}
				switch asObj.MyStr
				{
					case "Len"
						if asTyp is TypeArr
						{
							return new ObjInt(asTyp->{TypeArr^}.Size)
						}
					case "Base"
						if asTyp is TypeArr or asTyp is TypePoint it asTyp is TypeFatArr
						{
							return new ObjType(asTyp.Base)
						}
						return new ObjType(GTypeVoid)
					case "Name"
						return new ObjStr(GetConstString(asTyp.GetGoodName()))
					case "VTable"
						if asTyp is TypeClass
						{
							fType := GetFuncType(![asTyp],GTypeVoidP)
							return MakeSimpleCall(new BuiltIn2FuncGetVTable("",fType),null)
						}
				}
			}
		}
		if iter.Down is ParamNaturalCall
		{
			if iter.Down.Right.Right? is ObjIndent
			{
				asObj := iter.Down.Right.Right->{ObjIndent^}
				if asObj.MyStr == "ptr"
				{
					dwn := iter.Down
					dwn->SetType(ParamNaturalUnrefCall)
					iter.Down.Right = null
					return iter.Down
				}
			}
		}
		if iter.Down.Right.Right? is ObjIndent
		{	
			asObj := iter.Down.Right.Right->{ObjIndent^}
			if asObj.MyStr == "SetType" and iter.Down.Right.Right.Right != null
			{
				dt := iter.Down.GetType() 
				dtCl := TypeClass^()
				if dt != null
				{
					if dt is TypeClass
						dtCl = dt->{TypeClass^}
					if dt is TypePoint and dt.Base is TypeClass
						dtCl = dt.Base->{TypeClass^}
				}
				tt := ParseType(iter.Down.Right.Right.Right.Down)
				if dtCl == null
					iter.EmitError("to SetType, left object must be pointer to class")
				if tt == null or (not tt is TypeClass)
					iter.EmitError("to SetType, new type must be class")
				if dtCl.ToClass.vTypes.Size() == 0
					iter.EmitError("can not change type of non virtual object")
				if tt->{TypeClass^}.ToClass.vTypes.Size() == 0
					iter.EmitError("can not change type to non virtual type")
				setFunc := SetTypeToClass(dt,tt)
				if setFunc != null
				{
					iter.Down.Right = null
					return MakeSimpleCall(setFunc,iter.Down)
				}
			}
			if asObj.MyStr == "Type"
			{
				tp := iter.Down.GetType()
				if tp != null
					return new ObjType(tp)
			}
			if asObj.MyStr == "Fields"
			{
				tp := iter.Down.GetType()
				if tp != null
				{
					res := new FieldHolder(tp)
					if not iter.Down is ObjType
					{
						dataNode := iter.Down
						PopOutNode(iter.Down)
						res.Down = dataNode
						dataNode.Up = res
					}
					return res
				}
			}
		}
	}
	if iter.Down?.Right?.GetValue() == "is"
	{
		opr := iter.Down.Right
		if opr.Right?.GetValue() == "in"
		{
			firstType := opr.Left.GetType()
			if firstType? is TypePoint and firstType.Base is TypeClass
				and opr.Right.Right != null
			{
				scndType := ParseType(opr.Right.Right)
				if scndType? is TypeClass //TODO: virtual classes only
				{
					box := new FuncInputBox ; $temp

					box.itPars.Emplace(firstType,false)
					box.itConsts.Push(new ObjType(scndType)) //TODO: temporary new

					fnc := FindFunc("internalCheckClassInClass",opr,box^,false)
					if fnc != null
					{
						lft := iter.Down
						for 3 PopOutNode(lft.Right)
						return MakeSimpleCall(fnc,iter.Down)
					}
				}
			}

			
		}else{
			asCl1 := iter.Down.GetType()
			if asCl1 == null return null
			if not asCl1 is TypePoint return null
			if not asCl1.Base is TypeClass return null
			asCl2 := ParseType(iter.Down.Right.Right)
			if asCl2 == null or not asCl2 is TypeClass return null

			asNeed := asCl1.Base->{TypeClass^}.ToClass

			itBox := new FuncInputBox()  ; $temp

			itBox.itPars.Emplace(asCl1.Base,true)
			itBox.itConsts.Push(new ObjType(asCl2)) 

			func := asNeed.VirtualCheck.GetFunc(itBox^)
			if func == null return null
			PopOutNode(iter.Down.Right)
			PopOutNode(iter.Down.Right)
			return MakeSimpleCall(func,iter.Down)
		}
	}
	if iter.Down?.Right?.GetValue() == "=="
	{
		dw := iter.Down
		rr := iter.Down.Right.Right
		if rr != null
		{
			if dw is ObjType and rr is ObjType
			{
				t1 := dw->{ObjType^}.MyType
				if t1 == null
					return new ObjBool(false)
				t2 := rr->{ObjType^}.MyType
				if t2 == null
					return new ObjBool(false)
				return new ObjBool(t1 == t2)
			}
		}
	}
	if iter.Down?.Right?.GetValue() == ">="
	{
		dw := iter.Down
		rr := iter.Down.Right.Right
		if rr != null
		{
			if dw is ObjType and rr is ObjType
			{
				t1 := dw->{ObjType^}.MyType
				t2 := rr->{ObjType^}.MyType
				if t1? is TypeClass and t2? is TypeClass
				{
					t1 = t1.GetPoint()
					t2 = t2.GetPoint()
					t3 := TypeFight(t1,t2)
					return new ObjBool(t3 == t2)
				}
				return new ObjBool(false)
			}
		}
	}
	return null
}

FieldHolder := class extend ObjType
{
	this := !(Type^ tp) -> void
	{
		this."ObjType.this"(tp)
	}
}
