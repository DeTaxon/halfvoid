from Lex import *

TypeTable = []
NameTable = []

ClassTable = []
ClassStack = []


def PushT():
	ClassStack.append(len(ClassTable))
def PopT():
	ToSize = ClassStack.pop()
	while ToSize != len(ClassTable):
		ClassTable.pop()
def AddClass(NewC):
	ClassTable.append(NewC)

class NameChain:
    def __init__(self,N,T):
        self.Name = N
        self.Type = T

class Type:
    def __init__(self,B,T,N = None):
        self.Base = B
        self.Type  = T #standart funcp point array class
        self.Point = None
        self.Array = None
        self.Id = -1
        self.IsPoint = False
        self.Params = []
        if T == "standart":
            NameTable.append(NameChain(N,self))
    def PrintUse(self,F):
        if self.Type == "standart":
            F.write(self.Base)
        if self.Type in ["point","array"]:
            if self.Base.Id == 0:
                F.write("i8")
            else:
                self.Base.PrintUse(F)
            F.write("*")
        if self.Type == "funcp":
            self.Base.PrintUse(F)
            F.write("(")
            for i in range(len(self.Params)):
                if i != 0:
                    F.write(" , ")
                self.Params[i].PrintUse(F)
            F.write(")*")
    def PrintZero(self,F):
	if self.Type == "standart":
		F.write(self.Base)
		if self.Base in ["float","double"]:
			F.write(" 0.0")
		else:
			F.write(" 0")
	elif self.Type in ["point","array"]:
		self.PrintUse(F)
		F.write( " null")
    def GetName(self):
        if self.Type == "standart":
            return self.Base
        if self.Type in ["point","array"]:
            if self.Base.Id == 0:
                return "i8*"
            else:
                return self.Base.GetName() + "*"

		

def AddFuncPoint(Obj,NewName = None):
    FuncType = Type(None,"funcp")
    Pars = Obj.Extra[1].Extra
    if len(Pars) > 0:
        Pars.append(Token(',',','))
        PArrs = []
        j = 0
        while j < len(Pars):
            if Pars[j].Value == ',':
                if len(PArrs) == 1:
                    if PArrs[0].Value == "...":
                        FuncType.Params.append(ParamChain(GetType("...")))
                    #else: 
                        # error self.Params.append(ParamChain(None,PArrs[0]))
                elif len(PArrs) == 2:
                    FuncType.Params.append(ParseType(PArrs[0]))
                #else == 4   int x = 4
                PArrs = []
            else:
                PArrs.append(Pars[j])
            j += 1
    FuncType.Base = ParseType(Obj.Extra[4])
    FuncType.Id = len(TypeTable)
    TypeTable.append(FuncType) # broken
    if NewName != None:
        NameTable.append(NameChain(NewName,FuncType))
    return FuncType 

def GetVoidP():
    return GetType(["void","^"]).Id

def GetPoint(Bas):
    if Bas.Point == None:
        Bas.Point = Type(Bas,"point")
        TypeTable.append(Bas.Point)
        Bas.Point.Id = len(TypeTable)-1
        Bas.Point.IsPoint = True
    return Bas.Point
def GetArr(Bas):
    if Bas.Point == None:
        Bas.Point = Type(Bas,"point")
        TypeTable.append(Bas.Point)
        Bas.Point.Id = len(TypeTable)-1
        Bas.Point.IsPoint = True
    return Bas.Point

def GetType(A):
    if type(A) == type("str"):
        for N in NameTable:
            if N.Name == A:
                return N.Type
	for N in ClassTable:
		if N.Name == A:
			return N
        return None
    else:
        if len(A) == 0:
            return None
        Base = GetType(A[0])
        if Base == None:
            return None
        for i in range(1,len(A)):
            if A[i] == "^":
                Base = GetPoint(Base)
            if A[i] == "[]":
                Base = GetArr(Base)
        return Base
def ParseType(A):
    if A.Value == "id":
        return GetType(A.Extra)
    if A.Value in ["d^","d[]"]:
        Base = ParseType(A.Extra[0])
        It = A.Extra[1].Value
        if It == "^":
            Base = GetPoint(Base)
        if It == "[]":
            Base = GetArr(Base)
        return Base

def MakeSame(A,B):
    Base = GetType(A)
    if Base != None:
        NameTable.append(NameChain(B,Base))

def PutStand(Base,Name):
    PreType = Type(Base,"standart",Name)
    PreType.Id = len(TypeTable)
    TypeTable.append(PreType)

PutStand("void",  "void")
PutStand("...",   "...")
PutStand("i1",    "bool")
PutStand("i8",    "u8")
PutStand("i16",   "u16")
PutStand("i32",   "u32")
PutStand("i64",   "u64")
PutStand("i8",    "s8")
PutStand("i16",   "s16")
PutStand("i32",   "s32")
PutStand("i64",   "s64")
PutStand("float", "float")
PutStand("double","double")

MakeSame("u8","char")
MakeSame("s16","short")
MakeSame("s32","int")
MakeSame("s64","long")
MakeSame(["char","^"],"string")

ArrSum = []
for i in [8,16,32,64]:
	ArrSum.append(GetType("u{}".format(i)).Id)
	ArrSum.append(GetType("s{}".format(i)).Id)
for i in ["float","double"]:
	ArrSum.append(GetType(i).Id)


def ShouldChange(T1,T2):
	ToRet = ""
	if T1.Id not in ArrSum:
		return ""
	if T2.Id not in ArrSum:
		return ""
	Ind1 = ArrSum.index(T1.Id)
	Ind2 = ArrSum.index(T2.Id)
	if Ind1 == Ind2:
		return ToRet

	if (Ind1 >= 8) != (Ind2 >= 8):
		ToRet += "f" if Ind1 >= 8 else "i"
	if Ind1 % 2  != Ind2 % 2:
		ToRet += "s" if Ind1 % 2  == 0 else "z"
	if Ind1 / 2  != Ind2 / 2:
		ToRet += "u" if Ind1 < Ind2 else "d"
	return ToRet









