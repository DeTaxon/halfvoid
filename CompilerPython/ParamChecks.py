from Type import *
def GoodPoints(Par1,Par2):
    if Par1.Id == GetVoidP() and Par2.IsPoint:# and not Par2.Base.IsPoint:
       return True
    #if Par2.Id == GetVoidP() and Par1.IsPoint:# and not Par1.Base.IsPoint:
    #   return True
    return False

def NormalCheck(CParams,FParams,IsOper = False):
	for i in range(len(CParams)):
		FParams[i].Check()
		if FParams[i].Type.Id == GetType("...").Id:
			return True
		if i >= len(CParams):
			return False
		if CParams[i].Type.Id == FParams[i].Type.Id:
			continue
		if FParams[i].Type.Type == "point" and CParams[i].Type.Type == "fixed" and FParams[i].Type.Base.Id == CParams[i].Type.Base.Id:
			continue
		if CParams[i].Type.Type == "class" and FParams[i].Type.Type == "class" and CParams[i].Type.InFamily(FParams[i].Type):
			continue
		if CParams[i].Type.Type == "point" and CParams[i].Type.Base.Type == "class" and FParams[i].Type.Id == CParams[i].Type.Base.Id and FParams[i].IsRef:
			continue
		if GoodPoints(FParams[i].Type,CParams[i].Type):
			continue
		if not IsOper and "u" in ShouldChange(CParams[i].Type,FParams[i].Type):
			continue
		if FParams[i].Type.Type == "funcp":
			continue
		if FParams[i].Type.Id != CParams[i].Type.Id:
			return False
	return True

def PrisvCheck(CParams,FParams):
	for i in range(len(CParams)):
		FParams[i].Check()
		if FParams[i].Type.Id == GetType("...").Id:
			return True
		if i >= len(CParams):
			return False
		if FParams[i].Type.Type == "point" and CParams[i].Type.Type == "fixed" and FParams[i].Type.Base.Id == CParams[i].Type.Base.Id:
			continue
		if CParams[i].Type.Type == "class" and FParams[i].Type.Type == "class" and CParams[i].Type.InFamily(FParams[i].Type):
			continue
		if GoodPoints(FParams[i].Type,CParams[i].Type):
			continue
		if FParams[i].Type.Type == "funcp":
			continue
		if FParams[i].Type.Id != CParams[i].Type.Id:
			return False
	return True
def StCheck(CParams,FParams):
	for i in range(len(CParams)):
		FParams[i].Check()
		if GoodPoints(FParams[i].Type,CParams[i].Type):
			continue
		if FParams[i].Type.Type == "funcp":
			continue
		if FParams[i].Type.Id != CParams[i].Type.Id:
			return False
	return True
