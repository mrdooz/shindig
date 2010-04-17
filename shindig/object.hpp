#ifndef _OBJECT_HPP_
#define _OBJECT_HPP_

// Reference counted base class
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

// Shared ptr class for Objects
template<class T>
class RefPtr
{
public:
	RefPtr() : _obj(NULL) {}
	RefPtr(T* obj)
	{
		_obj = obj;
		_obj->add_ref();
	}

	~RefPtr()
	{
		if (_obj) {
			_obj->release();
			_obj = NULL;
		}
	}

	void operator=(T* obj) {
		attach(obj);
		_obj->add_ref();
	}

	void attach(T* obj) {
		if (_obj) {
			_obj->release();
		}
		_obj = obj;
	}

	T* release()
	{
		T* tmp = _obj;
		if (_obj) {
			_obj->release();
			_obj = NULL;
		}
		return tmp;
	}

	T* operator&()
	{
		return _obj;
	}

	T& operator*()
	{
		return *_obj;
	}

	T* operator->()
	{
		return _obj;
	}

private:
	T* _obj;
};

#endif
