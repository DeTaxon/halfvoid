
HashAVLMap.{@K,@V} := type HashSomeMap.{64,AVLMap.{K,V},K,V}

HashSomeMap := class .{@Buckets,@TreeType,@K,@V}
{
	bucks := TreeType[Buckets]
	itSize := int

	this := !() -> void
	{
		itSize = 0
		bucks[^]."this"()
	}
	nowBuck := !(K key) -> ref TreeType
	{
		itHash := Hash(key)
		return bucks[itHash % Buckets] 
	}
	TryFind := !(K key) -> V^
	{
		nowArr := ref nowBuck(key)
		return nowArr.TryFind(key)
	}
	"[]" := !(K key) .{} -> ref V
	{
		nowArr := ref nowBuck(key)
		if key in nowArr
		{
			//TODO: mempool
			return nowArr[key]
		}else{
			itSize += 1
			return nowArr[key]
		}
	}
	Remove := !(K key) .{} -> void
	{
		nowBuck(key).Remove(key)
	}
	Contain := !(K key) .{} -> bool
	{
		return nowBuck(key).Contain(key)
	}
	"in" := !(K key) .{} -> bool
	{
		return key in nowBuck(key)
	}
	"~For" := !() .{} -> CommonMapIterator.{BadPair.{K,V},K,V}
	{
		result."this"(bucks[0].itTree.Start)
		for i : Buckets
		{
			if i == 0 continue
			nStart := bucks[i].itTree.Start
			if nStart != null
			{
				if result.nNode == null
				{
					result.nNode = nStart
					result.DoAMove()
				}else{
					result.miniStack.Emplace(nStart,true) ; $temp
				}
			}
		}
	}
	Size := !() -> int
	{
		return itSize
	}
	Values := fake
	{
		"in" := !(V val) -> bool
		{
			if this[^] == val
				return true
			return false
		}
		Clone := !() .{} -> V[] 
		{
			res := new V[itSize]
			for frm : this, to : res
			{
				to = frm
			}
			return res
		}
		Get := !() -> V[]
		{
			return Clone() ; $temp
		}
	}
}
