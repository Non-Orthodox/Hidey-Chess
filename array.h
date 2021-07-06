#ifndef ARRAY_H
#define ARRAY_H

#include <vector>

// enum arrayMode_t {
// 	arrayMode_same,     // Allocated memory grows to the same size as the array length.
// 	arrayMode_double    // Allocated memory is doubled when the array length exceeds the allocated memory length.
// };

template<typename T>
class Array {
protected:
	std::vector<T> array;
	// size_t length = 0;
	// size_t memsize = 0;
	// arrayMode_t arrayMode = arrayMode_same;
	// bool noShrink = 0;  // Enabled: Don't shrink allocated memory when the array doesn't fit it.
public:
	Array() {}
	Array(size_t length) {
		// this.length = length;
	}
	~Array() {
		// delete[] array;
		// array = nullptr;
		// length = 0;
		// memsize = 0;
	}
	auto operator[](std::ptrdiff_t index) {
		return array[index];
	}
	void push(T value) {
		array.push_back(value);
	}
};

#endif // ARRAY_H
