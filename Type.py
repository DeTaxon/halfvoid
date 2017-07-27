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
        if T == "standart":
            NameTable.append(NameChain(N,self))
    def PrintUse(self,F):
        if self.Type == "standart":
            F.write(self.Base)
        if self.Type in ["point","array"]:
            self.Base.PrintUse(F)
            F.write("*")


def GetPoint(Bas):
    if Bas.Point == None:
        Bas.Point = Type(Bas,"point")
        TypeTable.append(Bas.Point)
    return Bas.Point
def GetArr(Bas):
    if Bas.Point == None:
        Bas.Point = Type(Bas,"point")
        TypeTable.append(Bas.Point)
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


TypeTable.append(Type("void","standart", "void"))
TypeTable.append(Type("...","standart",  "..."))
TypeTable.append(Type("i8","standart",   "u8"))
TypeTable.append(Type("i16","standart",  "u16"))
TypeTable.append(Type("i32","standart",  "u32"))
TypeTable.append(Type("i64","standart",  "u64"))
TypeTable.append(Type("i8","standart",   "s8"))
TypeTable.append(Type("i16","standart",  "s16"))
TypeTable.append(Type("i32","standart",  "s32"))
TypeTable.append(Type("i64","standart",  "s64"))
TypeTable.append(Type("float","standart","float"))
TypeTable.append(Type("double","standart","double"))

MakeSame("u8","char")
MakeSame("s16","short")
MakeSame("s32","int")
MakeSame("s64","long")
MakeSame(["char","^"],"string")

