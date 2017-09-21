from Lex import *

def UNext(Arr,Pos,Size,Name):
	Tok = Token(Name,[])
	Tok.Line = Arr[Pos].Line
	Tok.InFile = Arr[Pos].InFile
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

def RuleMetod(Arr,Pos):
	if Pos + 2 >= len(Arr):
		return False
	if Arr[Pos].Value not in datar:
		return False
	if Arr[Pos+1].Value != ".":
		return False
	if Arr[Pos+2].Value != "id":
		return False
	if Pos + 3 < len(Arr) and Arr[Pos+3].Value == "()":
		UNext(Arr,Pos,4,"d.d()")
	else:
		UNext(Arr,Pos,3,"d.d")
	return True
			
        

datar  = ["numi","numf","id","str","true","false"]
Box = ["d&","d^","d[]","d()","d<>","{}[]","d.d","d.d()","dm","db","ds","!()","dp","this","newstuf","d.{}","null","![]"]
datar += Box
ForFor = datar
ForFor += ["{}","daif","dawhile","ret","dafor"]

Rules = []
Rules.append(["dm",datar,['++','--']])

#Rules.append(["d.d()",datar,".","id","()"])
#Rules.append(["d.d",datar,".","id"])
Rules.append(RuleMetod)

Rules.append(["!{}","!","{}"])
Rules.append(["d.{}",datar,".","{}"])

Rules.append(["d^",datar,'^'])
Rules.append(["d[]",datar,'[]'])
Rules.append(["d&",datar,'&'])
Rules.append(["d()",datar,"()"])
Rules.append(["!()^","!","()","^","->",datar])
Rules.append(["!()","!","()","->","ref",datar,["{}","declare"]])
Rules.append(["!()","!","()","->",datar,["{}","declare"]])
Rules.append(["ds","()"])
Rules.append(["![]","!","[]"])
Rules.append(["dm",datar,["**"],datar])
Rules.append(["dm",datar,["*","/","%"],datar])
Rules.append(["dm",datar,["+","-"],datar])
Rules.append(["dm","-",datar])
Rules.append(["db",datar,["==","!=",">",">=","<","<="],datar])
Rules.append(["db","not",datar])
Rules.append(["newstuf","new",datar])
Rules.append(["db",datar,["and","or","xor"],datar])
Rules.append(["newconst","id","::",datar])
Rules.append(["dp",datar,["=","+=","-=","*=","/="],datar])
Rules.append(["fakeparam","id",":=",datar,"at",datar])
Rules.append(["newparams","id",",","id",",","id",",","id",":=",datar])
Rules.append(["newparams","id",",","id",",","id",":=",datar])
Rules.append(["newparams","id",",","id",":=",datar])
Rules.append(["newparam",["id","this","str"],":=",datar])
Rules.append(["newtype","id",":=","type","!()^"])
Rules.append(["classtemplate","id",":=","class","!{}","extend",datar,"{}"])
Rules.append(["newclass","id",":=","class","extend",datar,"{}"])
Rules.append(["classtemplate","id",":=","class","!{}","{}"])
Rules.append(["newclass","id",":=","class","{}"])
Rules.append(["ret","return",datar])
Rules.append(["daif","if",datar,ForFor,"else",ForFor])
Rules.append(["daif","if",datar,ForFor])
Rules.append(["dawhile","do",ForFor,"while",datar])
Rules.append(["dawhile","while",datar,ForFor])
Rules.append(["dafor","for","id",",","id",":",datar,ForFor])
Rules.append(["dafor","for","id",":",datar,ForFor])
Rules.append(["dafor","for",datar,ForFor])

for R in Rules:
    if type(R) is not type([]):
	continue
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
		if type(Rule) is type([]):
                	Resl = RuleIt(Rule,Arr,i)
		else:
			Resl = Rule(Arr,i)
		GotSome += Resl
		if not Resl:
                	i += 1
            if GotSome:
                break



