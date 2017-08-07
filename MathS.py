from Lex import *

def UNext(Arr,Pos,Size,Name):
    Tok = Token(Name,[])
    for i in range(Size):
        Tok.Extra.append(Arr[Pos+i])
    Arr[Pos] = Tok
    for i in range(1,Size):
        Arr.pop(Pos + 1)

def RuleIt( R, Arr,Pos):
    if Pos + len(R) - 1 > len(Arr):
        return False
    for  i in range (1,len(R)):
        if Arr[Pos + i - 1].Value not in R[i]:
            return False
    UNext(Arr,Pos,len(R) - 1,R[0])
    return True
#def RuleFunc(R 
        

datar  = ["numi","numf","id","str"]
Box = ["d&","d^","d[]","d()","d<>","{}[]","d.d()","dm","db","!()"]
datar += Box

Rules = []
Rules.append(["dm",datar,['++','--']])
Rules.append(["d^",datar,'^'])
Rules.append(["d[]",datar,'[]'])
Rules.append(["d.d()",datar,".",datar,"()"])
Rules.append(["d()",datar,"()"])
Rules.append(["!()","!","()","->",datar,["{}","declare"]])
Rules.append(["dm",datar,["**"],datar])
Rules.append(["dm",datar,["*","/","%"],datar])
Rules.append(["dm",datar,["+","-"],datar])
Rules.append(["db",datar,"==",datar])
Rules.append(["db",datar,["or","xor"],datar])
Rules.append(["db",datar,"and",datar])
Rules.append(["newparam","id",":=",datar])
Rules.append(["ret","return",datar])
#Rulse.append(["nummi"

for R in Rules:
    for i in range(1,len(R)):
        if type(R[i]) is str:
            R[i] = [R[i]]

#Rules = [RuleCall]

def SearchMath(Arr):
    GotSome = True

    while GotSome:
        GotSome = False
        for Rule in Rules:
            i = 0
            while i < len(Arr):
                GotSome += RuleIt(Rule,Arr,i)
                i += 1
            if GotSome:
                break



