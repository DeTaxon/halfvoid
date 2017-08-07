TypeTable = []
NameTable = []

class NameChain:
    def __init__(self,N,T):
        self.Name = N
        self.Type = T

class Type:
    def __init__(self,B,T,N = None):
        self.Base = B
        self.Type  = T #standart func point array
        self.Point = None
        self.Array = None
        self.Id = -1
        if T == "standart":
            NameTable.append(NameChain(N,self))
    def PrintUse(self,F):
        if self.Type == "standart":
            F.write(self.Base)
        if self.Type in ["point","array"]:
            self.Base.PrintUse(F)
            F.write("*")
    def GetName(self,F):
        if self.Type == "standart":
            return self.Base
        if self.Type in ["point","array"]:
            return self.Base.PrintUse(F) + "*"


def GetPoint(Bas):
    if Bas.Point == None:
        Bas.Point = Type(Bas,"point")
        TypeTable.append(Bas.Point)
        Bas.Point.Id = len(TypeTable)-1
    return Bas.Point
def GetArr(Bas):
    if Bas.Point == None:
        Bas.Point = Type(Bas,"point")
        TypeTable.append(Bas.Point)
        Bas.Point.Id = len(TypeTable)-1
    return Bas.Point

def GetType(A):
    if type(A) == type("str"):
        for N in NameTable:
            if N.Name == A:
                return N.Type
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



