#ifndef _OBJECT_HPP_
#define _OBJECT_HPP_

class Object
{
public:
	Object() : _ref_count(1) {}
	void add_ref() const 
	{ 
		InterlockedIncrement(&_ref_count); 
	}

	void release() const 
	{ 
		if (InterlockedDecrement(&_ref_count) == 0) {
			const_cast<Object*>(this)->destroy();
		}
	}
protected:
	int32_t ref_count() const { return _ref_count; }
	virtual void destroy() = 0;
private:
	mutable LONG _ref_count;
};

#endif
