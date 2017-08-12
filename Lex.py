import re
KeyWords = ["for","while","do","if","else","operator","it","this","and","or","xor","not","declare","operator","class","return","type"]
KeyWords += ["true","false"]
Symbols = "{}()[]?%^&!,;"
Digits = "0123456789"
Digits16 = Digits + "ABCDEFabcdef"

DoubleOpers =  [   "++",   "--",   "??","##"]
DoubleOpersR = [r"\+\+",r"\-\-",r"\?\?","##"]
Oper = "+-*/%=<>!~"

MoveIt =  ["=>",  "->",":=",    "...",   "..",  ".", r":",   "+=",   "-=",   "*=","/=","%=",   "**"]
MoveItR = ["=>",r"\->",":=",r"\.\.\.",r"\.\.",r"\.",r"\:",r"\+\=",r"\-\=",r"\*\=","/=","%=",r"\*\*"]

Cmprs = ["<=",">=","==","!=","<>"]

class Token:
    def __init__(self,Val,Ext = None):
        self.Value = Val
        self.Extra = Ext
    def Info(self):
        Simple = ["str","numi","numf","nump","id","=="]
        if self.Value in Simple:
            return "{} <{}>".format(self.Value,self.Extra)
        if self.Value in Symbols:
            return self.Value
        return self.Value

def TryLine(R,Line):
    m = re.match(R,Line)
    if m == None:
        return 0
    return len(m.group(0))
def GenTokens(Arr,Line):
    i = 0
    End = len(Line)
    while i < End:
        c = Line[i]
        PostLine = Line[i:]

        GotSome = False
        for Do in DoubleOpersR:
            Size = TryLine(Do,PostLine)
            if Size > 0:
                Arr.append(Token(DoubleOpers[DoubleOpersR.index(Do)]))
                GotSome = True
                i += Size
                break
        if GotSome:
            continue

        GotSome = False
        for Cmp in Cmprs:
            Size = TryLine(Cmp,PostLine)
            if Size > 0:
                Arr.append(Token("==",Cmp))
                i += Size
                GotSome = True
                break
        if GotSome:
            continue

        if c in Symbols:
            Arr.append(Token(c))
            i += 1
            continue
        if c == "\"":
            j = i + 1
            GotEnd = False
            while j < End and not GotEnd:
                if Line[j] == '"':
                    GotEnd = True
                elif Line[j] == '\\':
                    j += 1
                j += 1
            if j < End and GotEnd:
                GotStr = Line[i+1:j-1]
                Arr.append(Token("str",GotStr))
                i = j
                continue

        Size = TryLine(r"0x[0-9a-zA-Z]+",PostLine)
        if Size > 0:
            j = i + 2
            Val = 0
            while Line[j] in Digits16:
                Dig = Digits16.index(Line[j])
                if Dig >= 16:
                    Dig -= 6
                Val = Val*16 + Dig
                j += 1
            i += Size
            Arr.append(Token('numi',Val))
            continue

        Size = TryLine(r"[0-9]*\.[0-9]+p?",PostLine)
        if Size > 0:
            if Line[i + Size-1] == 'p':
                Arr.append(Token('nump',float(Line[i:i+Size-1])))
            else:
                Arr.append(Token('numf',float(Line[i:i+Size])))
            i += Size
            continue

        Size = TryLine("[0-9]+",PostLine)
        if Size > 0:
            Arr.append(Token('numi',int(Line[i:i+Size])))
            i += Size
            continue
        Size = TryLine("[@$]?[a-zA-Z_][a-zA-Z_0-9]*",PostLine)
        if Size > 0:
            PreAdd = Line[i:i + Size]
            if PreAdd in KeyWords:
                Arr.append(Token(PreAdd))
            else:
                Arr.append(Token('id',PreAdd))
            i += Size
            continue

        GotSome = False
        for It in MoveItR:
            Size = TryLine(It,PostLine)
            if Size > 0:
                Arr.append(Token(MoveIt[MoveItR.index(It)]))
                i += Size
                GotSome = True
                break
        if GotSome:
            continue
        if c in Oper:
            Arr.append(Token(c))
            i+= 1
            continue

        i += 1

