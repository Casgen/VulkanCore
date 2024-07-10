#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <optional>

template <typename T>
class Stack
{

  public:
    /**
     * A Stack-like structure for holding elements (FIFO). Can be resized.
     * @param capacity - Initial stack capacity.
     * @parama isCapped - If true, Ensures that the stack won't fill over it's capacity.
     */
    Stack(const size_t capacity = 2, bool isCapped = false) : isCapped(isCapped), capacity(capacity)
    {
        data = (T*) std::calloc(capacity, sizeof(T));
    }

    Stack operator=(const Stack& other) = delete;
    Stack(const Stack& other) = delete;

    Stack(Stack&& other)
    {
        if (this != &other)
        {
            this->data = other.data;
            this->isCapped = other.isCapped;
            this->capacity = other.capacity;
            this->index = other.index;
        }
    }

    Stack& operator=(Stack&& other)
    {
        if (this != &other)
        {
            delete[] data;

            this->data = other.data;
            this->capacity = other.capacity;
            this->isCapped = other.isCapped;
            this->index = other.index;
        }

        return *this;
    }

    ~Stack()
    {
        index = 0;
        capacity = 0;
        delete[] data;
    }

    bool Push(const T& element)
    {

        if (index >= capacity)
        {
            if (isCapped)
            {
                return false;
            }

            Resize();
        }

        data[index++] = element;
        return true;
    }

    size_t Count() const
    {
        return index;
    }

	std::optional<T> Pop()
    {
		if (isEmpty()) {
			return std::nullopt;
		}

        const T element = data[--index];

        T* prev = &data[index];
		*prev = NULL;

        return element;
    }

	bool isEmpty() const { return index == 0;}

  private:
    T* data = nullptr;
    size_t index = 0;
	size_t capacity = 0;
    bool isCapped = false;

    void Resize()
    {
        size_t newCapacity = capacity * 1.5f;
        T* newData = (T*) std::calloc(newCapacity, sizeof(T));

		std::memcpy(newData, data, capacity * sizeof(T));

        delete[] data;
        capacity = newCapacity;
    }
};
