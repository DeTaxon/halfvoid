
Rel := class .{@T}
{
	val := int
	"->{}" := !() .{T^} -> T^
	{
		res := this&->{void^}->{size_t}
		if val > 0
		{
			res += val
		}else{
			res -= -val
		}
		return res->{void^}->{T^}
	}
	"^" := !() -> ref T
	{
		return this->{T^}^
	}
	"=" := !(T^ newVal) -> void
	{
		val1 := newVal->{void^}->{size_t}
		val2 := this&->{void^}->{size_t}
		if val1 > val2
		{
			val = val1 - val2
		}else{
			val = val2 - val1
			val = -val
		}
	}
}
