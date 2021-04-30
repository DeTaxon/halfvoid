FuncCallOperator := !(Object^ iter) -> Object^
{
    if IsOper(iter.GetValue())
    {
        oper := iter.GetValue()
        if iter.Right == null
        {
            iter = iter.Left
            PopOutNode(iter.Right)
            return OperFunc(oper,iter.Up.Down,iter) //OneCall(oper,iter.Up,true)

        }else
        {
            iter = iter.Right

            if iter.Right != null
            {

                oName := ""sbt + oper

                if IsOper(iter[^].GetValue())
                {
                    oName << " " << it.GetValue()
                }
                bigOper := oName.Str() ; $temp
                if bigOper == "?: :" and iter.Right.Right != null
                {
                    strt := iter.Left.Left
                    typA := strt.GetType()
                    if typA == null or TypeCmp(typA,GTypeBool) == 255
                        strt.EmitError("Incorrect boolean type for ?:")
                    typB := iter.GetType()
                    if typB == null
                        strt.EmitError("Incorrect first input for ?:")
                    typC := iter.Right.Right.GetType()
                    if typC == null
                        strt.EmitError("Incorrect second input for ?:")
                    resType := TypeFight(typB,typC)
                    if resType == null
                        strt.EmitError("Types can not be united for ?:")
                    return new TrinaryOper(strt,resType)
                }
                iter.Up.Print(0)
                iter.EmitError("no more then binary allowed \n")
                return null
            } 
            if iter.GetType() != null
            {
                operNode := iter.Left
                PopOutNode(iter.Left)
                if oper == "in" and iter.Up != null
                {
                    ForInSwapNodes(iter.Left)
                }
                return  OperFunc(oper,iter.Up.Down,operNode) //OneCall(oper,iter.Up)
            }
            return null
        }
    }
    return null
}