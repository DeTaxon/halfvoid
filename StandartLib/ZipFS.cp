
vRepoObject := class
{
	objName := StringSpan
	upFolder := vRepoFolder^

	GetName := !() -> StringSpan { result = objName }
	
	GetUpFolder := !() -> vRepoFolder^ { return upFolder }
	GetPath := !() -> char^
	{
		itms := List.{vRepoObject^}() ; $temp

		iterUp := this&

		while iterUp != null
		{
			itms.PushFront(iterUp)
			iterUp = iterUp.upFolder
		}

		strB := StringBuilderTemporary.{256}()

		for it,i : itms
		{
			strB << it.objName
			if i != itms.Size() - 1
			{
				strB << "/"
			}
		}

		return strB.Str() ; $temp
	}
}

vRepoFolder := class extend vRepoObject
{
	examined := bool
	virtualFolder := bool
	subZipFolders := List.{vZipEntry^}
	subFolders := List.{vRepoFolder^}
	subFiles := List.{vRepoFile^}

	IsVirtual := !() -> bool { return virtualFolder }
}

vRepoFile := class extend vRepoObject
{
	ptrToRepo := vRepo^
	ptrToZip := vZipEntry^
	mFile := MappedFile
	fileSize := u64

	this := !() -> void
	{
		ptrToProxy = null
	}
	GetFile := !(char^ fileName) -> vRepoFile^
	{
		return ptrToRepo.GetFile(fileName,upFolder)
	}
	Map := !() -> u8^
	{
		if ptrToZip != null
		{
			return ptrToZip.Map()->{u8^}
		}

		mFile.Open(GetPath())

		return mFile.point->{u8^}

	}
	IsVirtual := !() -> bool 
	{
		return ptrToZip != null
	}
	Unmap := !() -> void
	{
		if ptrToZip != null
		{
			ptrToZip.Unmap()
		}else{
			mFile.Close()
		}
	}
	Size := !() -> size_t
	{
		if ptrToZip != null
		{
			return ptrToZip.Size()
		}
		return fileSize
	}
	IsValid := !() -> bool
	{
	}
}

vRepo := class
{
	rootFolder := vRepoFolder^
	repoMP := AllocOnlyMP.{4096,true}^
	ignZip := AVLSet.{u64}
	Init := !(char^ pathName)-> void
	{
		ignZip."this"()
		repoMP = new AllocOnlyMP.{4096,true}
		repoMP.Push()
		defer repoMP.Pop()
		rootFolder = new vRepoFolder
		startName := StrCopy(pathName)
		rootFolder.objName = StringSpan(startName)
	}

	AddZipRoot := !(char^ fileName) -> bool
	{
		repoMP.Push()
		defer repoMP.Pop()

		itP := Path(fileName)
		itId := itP.GetId()
		if itId == 0 return false

		ignZip << itId

		itObj := new ZipFile
		itObj.AnalizeFile(fileName)
		rootFolder.subZipFolders << itObj.zipRoot&

	}
	GetFile := !(char^ fileName)-> vRepoFile^
	{
		return GetFile(fileName,rootFolder)
	}
	ExamineFolder := !(vRepoFolder^ iterFolder) -> void
	{
		if not iterFolder.examined
		{
			iterFolder.examined = true

			zips := List.{char^}() ; $temp

			if not iterFolder.virtualFolder
			{
				fldPath := Path(iterFolder.GetPath())
				for subF : fldPath
				{
					if subF.IsFolder()
					{
						newStr := subF.Name().Str()
						newObj := new vRepoFolder
						newObj.objName = StringSpan(newStr)
						newObj.upFolder = iterFolder
						iterFolder.subFolders << newObj
					}else{
						itmId := subF.GetId()
						flSz := subF.Size()

						//if itmId in ignZip
						if ignZip.Contain(itmId)
						{
							continue
						}
						if subF.Get()[-4..0] == ".zip"
						{
							zips << StrCopy(subF.Get())
						}else{
							newStr := subF.Name().Str()
							newObj := new vRepoFile
							newObj.upFolder = iterFolder
							newObj.objName = StringSpan(newStr)
							newObj.ptrToRepo = this&
							newObj.fileSize = flSz
							iterFolder.subFiles << newObj
						}
					}
				}
				for z : zips
				{
					tt := new ZipFile
					tt.AnalizeFile(z)

					itPP :=Path(z)
					itP := itPP.Name()

					rpFld := null->{vRepoFolder^}

					if iterFolder.subFolders[^].objName == itP
					{
						rpFld = it
						break
					}
					if rpFld == null
					{
						rpFld = new vRepoFolder
						rpFld.objName = StringSpan(itP[0..-4].Str())
						rpFld.virtualFolder = true
						iterFolder.subFolders << rpFld
					}
					rpFld.subZipFolders << tt.zipRoot&
				}
			}

			for entrs : iterFolder.subZipFolders
			{
				for subItm : entrs.subFolders
				{
					found := false
					if iterFolder.subFiles[^].objName == subItm.objName
					{
						found = true
						break
					}
					if found continue

					if iterFolder.subFolders[^].objName == subItm.objName
					{
						found = true
						if subItm.realSize == 0
							it.subZipFolders << subItm
						break
					}
					if found continue

					if subItm.realSize == 0
					{
						rpFld := new vRepoFolder
						rpFld.objName = subItm.objName
						rpFld.virtualFolder = true
						iterFolder.subFolders << rpFld 
						rpFld.subZipFolders << subItm
					}else{
						newObj := new vRepoFile ; $pool
						newObj.upFolder = iterFolder
						newObj.objName = subItm.objName
						newObj.ptrToRepo = this&
						iterFolder.subFiles << newObj
						newObj.ptrToZip = subItm
					}
				}
			}
		}
	}

	GetFile := !(char^ fileName,vRepoFolder^ rrF) -> vRepoFile^
	{
		repoMP.Push()
		defer repoMP.Pop()
		
		itms := DivideStr(fileName,'/') ; $temp
		iterFolder := rrF

		for cheks,i : itms
		{
			if i == 0 and cheks == "." continue

			ExamineFolder(iterFolder)
			
			found := false
			if iterFolder.subFolders[^].objName == cheks
			{
				iterFolder = it
				found = true
				break
			}
			
			if found continue

			if iterFolder.subFiles[^].objName == cheks
			{
				return it
			}

			if cheks == ".."
			{
				iterFolder = iterFolder.upFolder
				if iterFolder == null
					return null
				continue
			}
			return null

		}
		return null

	}
}
