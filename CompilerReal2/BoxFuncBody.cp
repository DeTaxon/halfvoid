BoxFuncBody := class extend BoxFunc
{
	parsed := bool
	ItParams := LocalParam^[]
	InAlloc := int[]
	ExtraRetParam := FuncParam^
	Parent := BoxFuncBody^

	outLabel := BoxLabelAnon

	GetScope := virtual !() -> int { return ABox.ItId }

	ApplyParams := !(int count,string^ names, Type^^ pars,bool^ isRef) -> void
	{
		if count != 0
		{
			ItParams = new LocalParam^[count]
			InAlloc = new int[count]

			for i : count
			{
				if isRef[i]{
					InAlloc[i] = ABox.GetAlloc(pars[i].GetPoint())
				}else{
					InAlloc[i] = ABox.GetAlloc(pars[i])
				}
				ItParams[i] = new LocalParam(pars[i],InAlloc[i],isRef[i])
			}
		}
	}
	
	this := !(string^ names, TypeFunc^ fType,string SomeName, Object^ Stuf,bool IsSuf,Type^ metC,bool IsVirt) -> void
	{
		ABox.ItId = GetNewId()
		outLabel."this"()
		IsRetRef = fType.RetRef
		MyFuncParamNames = names
		FuncName = SomeName
		MethodType = metC
		IsVirtual = IsVirt

		if SomeName == "_hvEntryPoint"
		{
			OutputName = "main"
		}else
		{
			OutputName = GetFuncMangledName(SomeName,fType)
		}

		if SomeName == "new" IsStatic = true

		MyFuncType = fType

		TestRet(fType.RetType)

		if MyFuncType != null 
		{
			ApplyParams(MyFuncType.ParsCount,names,MyFuncType.Pars,MyFuncType.ParsIsRef)
		}

		if Stuf.GetValue() == "{}"
		{
			blk := new BoxBlock()
			blk.usePaths = true
			Down = blk
			Down.Line = Line
			Down.Up = this&
			if Stuf.Down != null
			{
				Down.Down = Stuf.Down
				Stuf.Down.SetUp(Down)
			}

		}else{
			ErrorLog.Push("CompilerError: function with weird body\n")
		}
		if IsInvalid EmitError("can not parse function header\n")

		if IsVirtual
		{
			ParseBlock()
		}

		IsSuffix = IsSuf
	}
	TestRet := !(Type^ t) -> void
	{
		if t != null
		{
			this.IsRetComplex = false
			if t is TypeArr or t is TypeClass
			{
				if not IsRetRef
				{
					this.IsRetComplex = true
					if ExtraRetParam == null
						ExtraRetParam = new FuncParam(ReturnName,t,true)
				}
			}
		}
	}
	this := !(Object^ inPars, Object^ inOutType,Object^ cons,bool RetRef, string SomeName, Object^ Stuf,bool IsSuf,Type^ metC,bool IsVirt,bool isRetSelf, Object^ anc) -> void
	{
		IsSelfReturn = isRetSelf
		ABox.ItId = GetNewId()
		outLabel."this"()
		IsRetRef = RetRef
		IsVirtual = IsVirt
		FuncName = SomeName
		MethodType = metC
	
		if SomeName == "new" {
			IsStatic = true
		}

		if SomeName == "~this"
		{
			if metC == null
			{
				EmitError("~this outside class\n")
			}else{
				asC := metC->{TypeClass^}
				toC := asC.ToClass
				if toC.ContainVirtual{
					IsVirtual = true
				}
			}
		}
		if inOutType != null inOutType.Up = this&

		IsInvalid = not ParseParams(inPars,inOutType,false)
		ParseConsts(cons)

		if MyFuncType != null TestRet(MyFuncType.RetType)

		if MyFuncType != null 
		{
			ApplyParams(MyFuncType.ParsCount,MyFuncParamNames,MyFuncType.Pars,MyFuncType.ParsIsRef)
		}
		if SomeName == "_hvEntryPoint"
		{
			OutputName = "main"
		}else
		{
			OutputName = GetFuncMangledName(FuncName,MyFuncType)
		}

		if Stuf.GetValue() == "{}"
		{
			Down = new BoxBlock()
			Down.Up = this&
			Down.Line = Line
			Down.Down = Stuf.Down
			Down->{BoxBlock^}.usePaths = true
			if Stuf.Down != null Stuf.Down.SetUp(Down)

		}else{
			ErrorLog.Push("CompilerError: function with weird body\n")
		}
		if IsInvalid inPars.EmitError("can not parse function header\n"sbt + "func name " + FuncName + "\n")

		IsSuffix = IsSuf
		for c : FuncName
		{
			if (not (c in 'a'..'z')) and (not (c in 'A'..'Z')) IsSuffix = false 
		}
		addItselfToClass(anc)
		if IsVirtual
		{
			ParseBlock()
		}
		if FuncName == "_hvEntryPoint"
		{
			ParseBlock()
		}
	}
	addItselfToClass := !(Object^ ancer) -> void
	{
		funcsHolder := GetBoxClassFuncsHolder(ancer)
		if funcsHolder == null
			return void
		funcsHolder^.methods[FuncName].Push(this&)

		if IsVirtual 
		{
			asNeed := GetUpClass(ancer)
			assert(asNeed != null)
			asNeed.PutVirtualFunc(FuncName,MyFuncType,this&)
		}
	}

	AddFuncCall := !(Object^ itm) -> void
	{
		itr := Down.Down

		if itr == null
		{
			Down.Down = itm
			itm.Up = Down
		}else{
			while itr.Right != null
			{
				itr = itr.Right
			}
			itr.Right = itm
			itm.Left = itr
			itm.Up = itr
		}
	}
	ParseBlock := virtual !() -> void
	{
		if not parsed 
		{
			if FuncName == "~this" and MethodType != null
			{
				pL := new FuncParam("this",MethodType,true)
				if this.MethodType is TypeClass
				{
					asCT := MethodType->{TypeClass^}
					asC := asCT.ToClass

					if asC.Parent != null
					{
						box5 := new FuncInputBox() 
						stp := asC.Parent
						box5.itPars.Emplace(stp.ClassType,true)
						func2 := asC.Parent.GetFunc("~this",box5^,true)

						if func2 != null
						{
							pCall := new ParamNaturalCall("",pL->{Object^})
							fCall := MakeSimpleCall(func2,pCall)
							AddFuncCall(fCall)
						}else{
							//EmitError("compiler error 82346\n")
						}
					}
					WorkBag.Push(this&,State_PrePrint)
				}
				
			}
			
			parsed = true
			WorkBag.Push(Down,State_Start)
			Down.Up = this&

		}
	}
	
	yieldInClass := int
	yieldInClassType := Type^
	AddYodler := virtual !(BoxReturn^ toAdd) -> int
	{
		YieldCheck()
		Yodlers.Push(toAdd)
		return Yodlers.Size()
	}
	YieldCheck := !() -> void
	{
		if Yodlers.Size() != 0
			return void

		yodlerInAlloc = ABox.GetAlloc(GTypeInt)

		if MethodType == null
		{
			if Yodlers.Size() != 0
				ABox.liveOnGlobal = true

		}else{
			bC := MethodType->{TypeClass^}.ToClass
			bC.Yodlers.Insert(this&)
		}
	}
	PrintGlobal := virtual !(TIOStream f) -> void
	{
		ABox.PrintGlobal(f)

		if MyFuncType.RetType != null and parsed
		{
			PrintGlobalSub(f)
			PrintFuncBodySkobs(f,MyFuncType,MyFuncParamNames,OutputName,null->{string},ABox.ItId)

			f << " #0 "

			doDebug := DebugMode
			if this& is BoxFuncBodyFromString
				doDebug = false

			if doDebug
			{
				f << " !dbg !" << ABox.ItId
			}

			f << " ; " << FuncName

			f << "\n{\n"
			dbgId := -1
			if DebugMode
				dbgId = CreateDebugCall(this&)

			if Yodlers.Size() != 0 and MethodType != null
			{
				cName := MethodType.GetName()
				yName := yieldInClassType.GetName()
				f << "%ClassYodlerPre = getelementptr " << cName << " , " << cName << "* %this , i32 0,i32 " << yieldInClass << "\n"
				f << "%ClassYodler = bitcast " << yName << "* %ClassYodlerPre to " << ABox.GetClassName() << "*\n"
				ABox.PrintBoxItems(f,"%ClassYodler",dbgId)
			}else{
				ABox.PrintAlloc(f,dbgId)
			}

			DeferFuncStart(f,dbgId)

			if InAlloc != null
			for i : MyFuncType.ParsCount
			{
					f << "store "
					f << MyFuncType.Pars[i].GetName()
					if MyFuncType.ParsIsRef[i] f << "*"
				if MyFuncParamNames != null and i < MyFuncParamNames->len
				{
					f << " %" << MyFuncParamNames[i] << " , "
				}else{
					f << " %Extra" << ABox.ItId << "Param" << i << " , "
				}
					f << MyFuncType.Pars[i].GetName()
					if MyFuncType.ParsIsRef[i] f << "*"
					f << "* %T" << InAlloc[i]
					if DebugMode and dbgId != -1
						f << ", !dbg !" << dbgId
					f << "\n"
				if DebugMode and MyFuncParamNames != null and i < MyFuncParamNames->len
				{
					if MyFuncType.ParsIsRef[i]
					{
						outId := CreateDbgLocVar(this&,MyFuncType.Pars[i],MyFuncParamNames[i],true)
						newId := CreateDebugCall(this&)
						if dbgId != -1 and outId != -1
						{
							f << "call void @llvm.dbg.declare(metadata " << MyFuncType.Pars[i].GetName() << "** %T" << InAlloc[i] << " , metadata !" << outId << " , metadata !DIExpression()) , !dbg !" << dbgId << "\n"
						}
					}else{
						outId := CreateDbgLocVar(this&,MyFuncType.Pars[i],MyFuncParamNames[i])
						if dbgId != -1 and outId != -1
						{
							f << "call void @llvm.dbg.declare(metadata " << MyFuncType.Pars[i].GetName() << "* %T" << InAlloc[i] << " , metadata !" << outId << " , metadata !DIExpression()) , !dbg !" << dbgId << "\n"
						}
					}
				}
			}
			if DebugMode 
			{
				PrintDebugDeclare(f,null,null->{char^})
			}
			
			if (not this.IsRetComplex) and (MyFuncType.RetType != GetType("void")) and (MyFuncType.RetType.GetName() != "void")
			{
				RT := MyFuncType.RetType
				f << "%Result = alloca " << RT.GetName()
				if MyFuncType.RetRef f << "*"
				f << "\n"
				if MyFuncType.RetRef
				{
					f << "store " << RT.GetName() << "* null , " << RT.GetName() << "** %Result\n"
				}else{
					if IsBool(RT)
					{
						f << "store i1 0,i1* %Result\n"
					}else
					if IsInt(RT)
					{
						f << "store " << RT.GetName() << " 0 , " << RT.GetName() << "* %Result\n"
					}else
					if IsFloat(RT)
					{
						f << "store " << RT.GetName() << " 0.0 , " << RT.GetName() << "* %Result\n"
					}else
					if RT is TypePoint or RT is TypeFatArr
					{
						f << "store " << RT.GetName() << " null , " << RT.GetName() << "* %Result\n"
					}
				}
			}
			if Yodlers.Size() != 0
			{
				f << "%Yodler = getelementptr i32 , i32* %T" << yodlerInAlloc << ",i32 0\n"
				f << "%StartYield = load i32, i32* %Yodler\n"
				f << "switch i32 %StartYield, label %Yield0 ["
				for i : Yodlers.Size() + 1
					f << "i32 " << i << ", label %Yield" << i << "\n"
				f << "]\n"
				f << "Yield0:\n"
			}

			PrintABoxExtra(f)
			
			Down[^].PrintInBlock(f)

			f << "br label %" << outLabel.GetLabel() << "\n"
			outLabel.PrintLabel(f)

			DeferFuncEnd(f,dbgId)


			if MyFuncType.RetType == GTypeVoid or this.IsRetComplex
			{
				f << "ret void\n"
			}else{
				retTypeName := MyFuncType.RetType.GetName()
				if MyFuncType.RetRef retTypeName = (""sbt + retTypeName + "*" <-)
				f << "%ResultItem = load " << retTypeName << " , " << retTypeName << "* %Result\n"
				f << "ret " << retTypeName << " " << "%ResultItem\n"
			}

			f << "}\n"

			if doDebug
			{	
				iter := Up
				if iter != null
				{
					while iter.Up != null
					{
						iter = iter.Up
					}
				}

				resFName := ""sbt + FuncName
				tmplVars := HybridQueue.{int,8}() ; $temp
				for val,k : ItAttrs
				{
					if tmplVars.Size() == 0 resFName << ".{"
					if tmplVars.Size() > 1 resFName << ","

					if val is ObjBool
					{
						asBool := val->{ObjBool^}
						nId := GetNewId()
						f <<"!" << nId <<  " = !DITemplateValueParameter(name:\""<< k <<"\", type: !"<< GTypeBool.metaId<<",value: i8 "
						if asBool.MyBool f << "1" else f << "0"
						f << ")\n";
						tmplVars.Push(nId)

						resFName << k << "="
						if asBool.MyBool resFName << "1" else resFName << "0"
					}
				}
				tmplId := -1
				if tmplVars.Size() != 0
				{
					resFName << "}"

					tmplId = GetNewId()
					f << "!" << tmplId << " = !{"
					for val,i : tmplVars
					{
						if i > 0 f << ","
						f << "!" << val
					}
					f << "}\n"
				}



				if iter != null
				{
					asN := iter->{BoxFile^}
					f << "!" << ABox.ItId << " = distinct !DISubprogram(name:\"" << resFName << "\","
					f << "linkageName:\"" << OutputName << "\","
					f << "scope: !" << asN.fileId << " , file: !" << asN.fileId
					f << ",type: !" << MyFuncType.metaId 
					if Line != null
					{
						f << ", line: " << Line.LinePos	
						f << ", scopeLine: " << Line.LinePos	
					}
					f << ", unit: !" << cuId
					f << ", flags: DIFlagPrototyped"
					f << ", spFlags: DISPFlagDefinition"
					if tmplId != -1
						f << ", templateParams:!"<<tmplId
					f << ")\n"

				}
			}
		}
	}
	GetOutPath := virtual !(Object^ item, int typ,int size) -> BoxLabel^
	{
		return outLabel&
	}

	DoTheWork := virtual !(int pri) -> void
	{
		if pri == State_Start
		{
			WorkBag.Push(this&,State_Syntax)
		}

		if pri == State_Syntax
		{
			//SyntaxCompress(this&,PriorityData)
			//UnboxParams(this.Down)
			WorkBag.Push(this&,State_ErrorCheck)

		}
		if pri == State_ErrorCheck
		{
			if MyFuncType == null
			{
				ErrorLog.Push("wut?\n")
			}else{
				if MyFuncType.RetType == null
					SetReturnType(GetType("void"))
			}
		}
		if pri == State_CheckDefer
		{
			ParseDefer()
		}
		if pri == State_PrePrint
		{
			asCT := MethodType->{TypeClass^}
			asC := asCT.ToClass
					
			i := asC.NotMineParams
			EndS := asC.Params.Size()
			for nowField,i : asC.Params
			{
				sBug := nowField->{Object^}
				itCPre := sBug.GetType()
				if itCPre is TypeClass
				{
					itC := itCPre->{TypeClass^}.ToClass

					box3 := new FuncInputBox()

					box3.itPars.Emplace(itCPre,true)
					func3 := itC.GetFunc("~this",box3^,true)
					if func3 != null
					{
						box4 := new FuncInputBox() 
						box4.itConsts.Push(new ObjStr(nowField.ItName))
						fnc := asC.AutoFieldTemplate
						pr := fnc.GetPriority(box4^)
						if pr == 0
						{
							asT := fnc->{BoxTemplate^}
							funcCl := asT.GetFunc(box4^)
							funcCll := MakeSimpleCall(funcCl,null->{Object^})

							exF := MakeSimpleCall(func3,funcCll)
							AddFuncCall(exF)
						}
						//exF := MakeSimpleCall(func3,pCall2)
						//AddFuncCall(exF)
					}else{
						EmitError("compiler error 2345\n")
					}
				}
			}
		}
	}
}
