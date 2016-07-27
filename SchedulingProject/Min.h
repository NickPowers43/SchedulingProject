#pragma once


template<typename K, typename V>
class Min
{
public:
	Min(K key, V val)
	{
		Max::val = val;
	}
	bool Compare(K key, V val)
	{
		{
			Min::key = key;
			Min::val = val;
			return true;
		}
		else
		{
			return false;
		}
	}

private:
	V val;
	K key;
};
