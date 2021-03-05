#pragma once

template<class T>
class SharedPtr {
public:
	explicit SharedPtr(T *ptr = 0) : counter(new int), ptr_(ptr) {
		*counter = 1;
	}

	~SharedPtr() {
		if (*counter == 1) {
			delete ptr_;
			delete counter;
		} else
			(*counter)--;
	}

	SharedPtr(const SharedPtr &sh_ptr) : counter(0), ptr_(0) {
		*this = sh_ptr;
	}

	SharedPtr &operator=(const SharedPtr &sh_ptr) {
		if (this == &sh_ptr)
			return *this;
		if (counter)
			if (--(*counter) == 0) {
				delete ptr_;
				delete counter;
			}
		*(sh_ptr.counter) += 1;
		counter = sh_ptr.counter;
		ptr_ = sh_ptr.ptr_;
		return *this;
	}

	T *get() const {
		return ptr_;
	}

	void reset(T *ptr = 0) {
		(*counter)--;
		if (*counter == 0) {
			delete ptr_;
			delete counter;
		}
		counter = new int;
		*counter = 1;
		ptr_ = ptr;
	}

	T &operator*() const {
		return *ptr_;
	}

	T *operator->() const {
		return ptr_;
	}

private:
	int *counter;
	T *ptr_;
};