#ifndef INC_3DGAME_AUTO_ARRAY_H
#define INC_3DGAME_AUTO_ARRAY_H

#include <stdexcept>
#include <cstdlib>
#include <initializer_list>
#include <cassert>
#include <iostream>
#include <iterator>
#include <array>

namespace bice
{

    template<typename TYPE, std::uint64_t SIZE>
    class ConstArrayIterator
    {
    public:

        using iterator_category   = std::random_access_iterator_tag;
        using value_type          = TYPE;
        using difference_type     = std::int64_t;
        using pointer             = const TYPE*;
        using reference           = const TYPE&;

        ConstArrayIterator() noexcept :
                index{}, ptr{} {}

        explicit ConstArrayIterator(const TYPE* ptr_, const std::uint64_t index_ = 0)noexcept :
                index(index_), ptr(ptr_) {}

        [[nodiscard]] constexpr reference operator * () const noexcept
        {

            return *ptr;
        }

        [[nodiscard]] constexpr pointer operator -> () const noexcept
        {
            return ptr;
        }

        [[nodiscard]] constexpr reference operator [] (const difference_type off) const noexcept
        {
            return *(ptr + off);
        }

        [[nodiscard]] constexpr bool operator == (const ConstArrayIterator<TYPE, SIZE>& other) const noexcept
        {
            return (index == other.index && ptr == other.ptr);
        }

        [[nodiscard]] constexpr bool operator != (const ConstArrayIterator<TYPE, SIZE>& other) const noexcept
        {
            return (index != other.index || ptr != other.ptr);
        }

        [[nodiscard]] constexpr bool operator > (const ConstArrayIterator<TYPE, SIZE>& other) const noexcept
        {
            return (index > other.index && ptr > other.ptr);
        }

        [[nodiscard]] constexpr bool operator >= (const ConstArrayIterator<TYPE, SIZE>& other) const noexcept
        {
            return (index > other.index && ptr > other.ptr) || (index == other.index && ptr == other.ptr);
        }

        [[nodiscard]] constexpr bool operator < (const ConstArrayIterator<TYPE, SIZE>& other) const noexcept
        {
            return (index < other.index && ptr < other.ptr);
        }

        [[nodiscard]] constexpr bool operator <= (const ConstArrayIterator<TYPE, SIZE>& other) const noexcept
        {
            return (index < other.index && ptr < other.ptr) || (index == other.index && ptr == other.ptr);
        }

        constexpr ConstArrayIterator<TYPE, SIZE>& operator ++ () noexcept
        {
            assert(index + 1 >= 0 && index + 1 <= SIZE);
            ++index;
            ++ptr;
            return *this;
        }

        constexpr ConstArrayIterator<TYPE, SIZE> operator ++ (int) noexcept
        {
            assert(index + 1 >= 0 && index <= SIZE);
            ConstArrayIterator<TYPE, SIZE> temp = *this;
            ++ptr;
            ++index;
            return temp;
        }

        constexpr ConstArrayIterator<TYPE, SIZE>& operator -- () noexcept
        {
            assert(index - 1 >= 0 && index - 1 <= SIZE);
            --ptr;
            --index;
            return *this;
        }

        constexpr ConstArrayIterator<TYPE, SIZE> operator -- (int) noexcept
        {
            assert(index - 1 >= 0 && index - 1 <= SIZE);
            ConstArrayIterator<TYPE, SIZE> temp = *this;
            --ptr;
            --index;
            return temp;
        }

        [[nodiscard]] constexpr ConstArrayIterator<TYPE, SIZE> operator + (const difference_type off) const noexcept
        {
            assert((index + off) <= SIZE && (index + off) >= 0);
            return ConstArrayIterator<TYPE, SIZE>(ptr + off, index + off);
        }

        constexpr ConstArrayIterator<TYPE, SIZE>& operator += (const difference_type off) noexcept
        {
            assert((index + off) <= SIZE && (index + off) >= 0);
            ptr += off;
            index += off;
            return *this;
        }

        [[nodiscard]] constexpr ConstArrayIterator<TYPE, SIZE> operator - (const difference_type off) const noexcept
        {
            assert((index - off) >= 0 && (index - off) <= SIZE);
            return ConstArrayIterator<TYPE, SIZE>(ptr - off, index - off);
        }

        [[nodiscard]] constexpr difference_type operator - (const ConstArrayIterator<TYPE, SIZE>& other) const noexcept
        {
            assert((index - other.index) >= 0 && (index - other.index) <= SIZE);
            return index - other.index;
        }

        constexpr ConstArrayIterator<TYPE, SIZE>& operator -= (const difference_type off) noexcept
        {
            assert((index - off) >= 0 && (index - off) <= SIZE);
            ptr -= off;
            index -= off;
            return *this;
        }

        [[nodiscard]] [[nodiscard]] constexpr pointer unwrapped() const noexcept
        {
            return ptr;
        }

    private:

        std::uint64_t index;
        pointer ptr;
    };

    template<typename TYPE, std::uint64_t SIZE>
    class ArrayIterator
    {
    public:

        using iterator_category   = std::random_access_iterator_tag;
        using value_type          = TYPE;
        using difference_type     = std::int64_t;
        using pointer             = TYPE*;
        using reference           = TYPE&;

        ArrayIterator() noexcept :
                index{}, ptr{} {}

        explicit ArrayIterator(pointer ptr_, const std::uint64_t index_ = 0) noexcept :
                index(index_), ptr(ptr_) {}

        [[nodiscard]] constexpr reference operator * () const noexcept
        {
            return *ptr;
        }

        [[nodiscard]] constexpr pointer operator -> () const noexcept
        {
            return ptr;
        }

        [[nodiscard]] constexpr reference operator [] (const difference_type off) const noexcept
        {
            return *(ptr + off);
        }

        [[nodiscard]] constexpr bool operator == (const ArrayIterator<TYPE, SIZE>& other) const noexcept
        {
            return index == other.index && ptr == other.ptr;
        }

        [[nodiscard]] constexpr bool operator != (const ArrayIterator<TYPE, SIZE>& other) const noexcept
        {
            return index != other.index && ptr != other.ptr;
        }

        [[nodiscard]] constexpr bool operator > (const ArrayIterator<TYPE, SIZE>& other) const noexcept
        {
            return index > other.index && ptr > other.ptr;
        }

        [[nodiscard]] constexpr bool operator >= (const ArrayIterator<TYPE, SIZE>& other) const noexcept
        {
            return (index > other.index && ptr > other.ptr) || (index == other.index && ptr == other.ptr);
        }

        [[nodiscard]] constexpr bool operator < (const ArrayIterator<TYPE, SIZE>& other) const noexcept
        {
            return index < other.index && ptr < other.ptr;
        }

        [[nodiscard]] constexpr bool operator <= (const ArrayIterator<TYPE, SIZE>& other) const noexcept
        {
            return (index < other.index && ptr < other.ptr) || (index == other.index && ptr == other.ptr);
        }

        constexpr ArrayIterator<TYPE, SIZE>& operator ++ () noexcept
        {
            assert((index + 1 >= 0) && (index + 1 <= SIZE));
            ++index;
            ++ptr;
            return *this;
        }

        constexpr ArrayIterator<TYPE, SIZE> operator ++ (int) noexcept
        {
            assert((index + 1 >= 0) && (index + 1 <= SIZE));
            ArrayIterator<TYPE, SIZE> temp = *this;
            ++ptr;
            ++index;
            return temp;
        }

        constexpr ArrayIterator<TYPE, SIZE>& operator -- () noexcept
        {
            assert((index - 1 >= 0) && (index - 1 <= SIZE));
            --index;
            --ptr;
            return *this;
        }

        constexpr ArrayIterator<TYPE, SIZE> operator -- (int) noexcept
        {
            assert((index - 1 >= 0) && (index - 1 <= SIZE));
            ArrayIterator<TYPE, SIZE> temp = *this;
            --ptr;
            --index;
            return temp;
        }

        [[nodiscard]] constexpr ArrayIterator<TYPE, SIZE> operator + (const difference_type off) const noexcept
        {
            assert((index + off) <= SIZE && (index + off) >= 0);
            return ArrayIterator<TYPE, SIZE>(ptr + off, index + off);
        }

        constexpr ArrayIterator<TYPE, SIZE>& operator += (const difference_type off) noexcept
        {
            assert((index + off >= 0) && (index + off <= SIZE));
            ptr += off;
            index += off;
            return *this;
        }

        [[nodiscard]] constexpr ArrayIterator<TYPE, SIZE> operator - (const difference_type off) const noexcept
        {
            assert((index - off >= 0) && (index - off <= SIZE));
            return ArrayIterator<TYPE, SIZE>(ptr - off, index - off);
        }

        [[nodiscard]] constexpr difference_type operator - (const ArrayIterator<TYPE, SIZE>& other) const noexcept
        {
            assert((index - other.index) >= 0 && (index - other.index) <= SIZE);
            return index - other.index;
        }

        constexpr ArrayIterator<TYPE, SIZE>& operator -= (const difference_type off) noexcept
        {
            assert((index - off) >= 0 && (index - off) <= SIZE);
            ptr -= off;
            index -= off;
            return *this;
        }

        [[nodiscard]] constexpr const pointer unwrapped() const noexcept
        {
            return reinterpret_cast<const pointer>(ptr);
        }

    private:

        std::uint64_t index;
        pointer ptr;
    };

    template<typename TYPE, size_t SIZE>
    struct Array
    {
        using value_type               = TYPE;
        using size_type                = std::uint64_t;
        using difference_type          = std::int64_t;
        using reference                = TYPE&;
        using const_reference          = const TYPE&;
        using pointer                  = TYPE*;
        using const_pointer            = const TYPE*;

        using iterator                 = ArrayIterator<TYPE, SIZE>;
        using const_iterator           = ConstArrayIterator<TYPE, SIZE>;

        using reverse_iterator         = std::reverse_iterator<iterator>;
        using const_reverse_iterator   = std::reverse_iterator<const_iterator>;

        value_type elements[SIZE];

        [[nodiscard]] inline constexpr size_type size() const noexcept
        {
            return SIZE;
        }

        [[nodiscard]] inline constexpr size_type max_size() const noexcept
        {
            return SIZE;
        }

        [[nodiscard]] inline constexpr bool empty() const noexcept
        {
            return false;
        }

        [[nodiscard]] constexpr inline pointer begin_ptr() noexcept
        {
            return elements;
        }

        [[nodiscard]] constexpr inline pointer end_ptr() noexcept
        {
            return &elements[SIZE];
        }

        [[nodiscard]] constexpr inline const_pointer cbegin_ptr() const noexcept
        {
            return reinterpret_cast<const_pointer>(elements);
        }

        [[nodiscard]] constexpr inline const_pointer cend_ptr() const noexcept
        {
            return reinterpret_cast<const_pointer>(&elements[SIZE]);
        }

        [[nodiscard]] constexpr iterator begin() noexcept
        {
            return iterator(elements, 0);
        }

        [[nodiscard]] constexpr iterator end() noexcept
        {
            return iterator(&elements[SIZE], SIZE);
        }

        [[nodiscard]] constexpr const_iterator cbegin() const noexcept
        {
            return const_iterator(elements, 0);
        }

        [[nodiscard]] constexpr const_iterator cend() const noexcept
        {
            return const_iterator(&elements[SIZE], SIZE);
        }

        [[nodiscard]] constexpr reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(end());
        }

        [[nodiscard]] constexpr reverse_iterator rend() noexcept
        {
            return reverse_iterator(begin());
        }

        [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept
        {
            return const_reverse_iterator(cend());
        }

        [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept
        {
            return const_reverse_iterator(cbegin());
        }

        [[nodiscard]] constexpr reference at(const size_type index)
        {
            if (index >= 0 && index < SIZE)
            {
                return elements[index];
            } else
            {
                throw std::out_of_range("Index out of bound");
            }
        }

        [[nodiscard]] constexpr const_reference at(const size_type index) const
        {
            if (index >= 0 && index < SIZE)
            {
                return elements[index];
            } else
            {
                throw std::out_of_range("Index out of bound");
            }
        }

        [[nodiscard]] constexpr reference operator [] (const size_type index) noexcept
        {
            return elements[index];
        }

        [[nodiscard]] constexpr const_reference operator [] (const size_type index) const noexcept
        {
            return elements[index];
        }

        [[nodiscard]] constexpr reference front() noexcept
        {
            return elements[0];
        }

        [[nodiscard]] constexpr const_reference front() const noexcept
        {
            return elements[0];
        }

        [[nodiscard]] constexpr reference back() noexcept
        {
            return elements[SIZE - 1];
        }

        [[nodiscard]] constexpr const_reference back() const noexcept
        {
            return elements[SIZE - 1];
        }

        [[nodiscard]] constexpr pointer data() noexcept
        {
            return elements;
        }

        [[nodiscard]] constexpr const_pointer data() const noexcept
        {
            return const_cast<const_pointer>(elements);
        }

        constexpr void fill(const_reference value)
        {
            for(size_type idx = 0; idx < SIZE; ++idx)
            {
                elements[idx] = value;
            }
        }

        constexpr void swap(Array<TYPE, SIZE>& otherArray) noexcept
        {
            value_type temp;

            for(size_type idx = 0; idx < SIZE; ++idx)
            {
                temp = elements[idx];
                elements[idx] = otherArray.elements[idx];
                otherArray.elements[idx] = temp;
            }
        }

        template<size_t SUB_SIZE>
        [[nodiscard]] constexpr Array<TYPE, SUB_SIZE> sub_array(const size_type startIndex)
        {
            if (startIndex >= 0 && startIndex + SUB_SIZE <= SIZE)
            {
                if (SUB_SIZE <= SIZE && SUB_SIZE > 0)
                {
                    Array<TYPE, SUB_SIZE> subArray{};

                    const size_type endIndex = startIndex + SUB_SIZE;
                    size_type subIndex = 0;

                    for(size_t index = startIndex; index < endIndex; ++index)
                    {
                        subArray.elements[subIndex] = elements[index];
                        ++subIndex;
                    }

                    return subArray;
                } else
                {
                    throw std::out_of_range("The size of the subarray exceeds the size of the original byteArray");
                }

            } else
            {
                throw std::out_of_range("Index out of bound original byteArray");
            }
        }

        template<size_t SUB_SIZE>
        [[nodiscard]] constexpr Array<TYPE, SUB_SIZE> sub_array_no_checks(const size_type startIndex) noexcept
        {
            Array<TYPE, SUB_SIZE> subArray{};

            const size_type endIndex = startIndex + SUB_SIZE;

            size_type subIndex = 0;
            for(size_t index = startIndex; index < endIndex; ++index)
            {
                subArray.elements[subIndex] = elements[index];
                ++subIndex;
            }

            return subArray;
        }

    };

    template<typename TYPE>
    struct Array<TYPE, 0>
            {

        using value_type = TYPE;
        using size_type = std::uint64_t;
        using difference_type = std::int64_t;
        using reference = TYPE &;
        using const_reference = const TYPE &;
        using pointer = TYPE *;
        using const_pointer = const TYPE *;

        using iterator = ArrayIterator<TYPE, 0>;
        using const_iterator = ConstArrayIterator<TYPE, 0>;

        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        [[nodiscard]] constexpr size_type size() const noexcept {
            return 0;
        }

        [[nodiscard]] constexpr size_type max_size() const noexcept {
            return 0;
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return true;
        }

        [[nodiscard]] constexpr inline pointer begin_ptr()
        {
            throw std::out_of_range("Array is empty");
        }

        [[nodiscard]] constexpr inline pointer end_ptr()
        {
            throw std::out_of_range("Array is empty");
        }

        [[nodiscard]] constexpr inline const_pointer cbegin_ptr() const
        {
            throw std::out_of_range("Array is empty");
        }

        [[nodiscard]] constexpr inline const_pointer cend_ptr() const
        {
            throw std::out_of_range("Array is empty");
        }

        [[nodiscard]] constexpr iterator begin()
        {
            throw std::out_of_range("Array is empty");
            return iterator();
        }

        [[nodiscard]] constexpr iterator end()
        {
            throw std::out_of_range("Array is empty");
            return iterator();
        }

        [[nodiscard]] constexpr const_iterator cbegin() const
        {
            throw std::out_of_range("Array is empty");
            return const_iterator();
        }

        [[nodiscard]] constexpr const_iterator cend() const
        {
            throw std::out_of_range("Array is empty");
            return const_iterator();
        }

        [[nodiscard]] constexpr reverse_iterator rbegin()
        {
            throw std::out_of_range("Array is empty");
            return reverse_iterator(end());
        }

        [[nodiscard]] constexpr reverse_iterator rend()
        {
            throw std::out_of_range("Array is empty");
            return reverse_iterator(begin());
        }

        [[nodiscard]] constexpr const_reverse_iterator crbegin() const
        {
            throw std::out_of_range("Array is empty");
            return const_reverse_iterator(cend());
        }

        [[nodiscard]] constexpr const_reverse_iterator crend() const
        {
            throw std::out_of_range("Array is empty");
            return const_reverse_iterator(cbegin());
        }

        [[nodiscard]] constexpr reference at(const size_type index)
        {
            throw std::out_of_range("Array is empty");
        }

        [[nodiscard]] constexpr const_reference at(const size_type index) const
        {
            throw std::out_of_range("Array is empty");
        }

        [[nodiscard]] constexpr reference operator[](const size_type index)
        {
            throw std::out_of_range("Array is empty");
        }

        [[nodiscard]] constexpr const_reference operator[](const size_type index) const
        {
            throw std::out_of_range("Array is empty");
        }

        [[nodiscard]] constexpr reference front()
        {
            throw std::out_of_range("Array is empty");
        }

        [[nodiscard]] constexpr const_reference front() const
        {
            throw std::out_of_range("Array is empty");
        }

        [[nodiscard]] constexpr reference back()
        {
            throw std::out_of_range("Array is empty");
        }

        [[nodiscard]] constexpr const_reference back() const
        {
            throw std::out_of_range("Array is empty");
        }

        [[nodiscard]] constexpr pointer data()
        {
            throw std::out_of_range("Array is empty");
        }

        [[nodiscard]] constexpr const_pointer data() const
        {
            throw std::out_of_range("Array is empty");
        }

        constexpr void fill(const_reference value) noexcept {}

        constexpr void swap(Array<TYPE, 0> &otherArray) noexcept {}

        [[nodiscard]] constexpr Array<TYPE, 0> sub_array(const size_type startIndex) {
            return Array<TYPE, 0>{};
        }

        [[nodiscard]] constexpr Array<TYPE, 0> sub_array_no_checks(const size_type startIndex) {
            return Array<TYPE, 0>{};
        }

    };

    template<typename TYPE, size_t SIZE>
    constexpr bool operator == (const Array<TYPE, SIZE>& array_1, const Array<TYPE, SIZE>& array_2) noexcept
    {
        auto equal = true;

        for(std::uint64_t idx = 0; idx < SIZE; ++idx)
        {
            if (array_1.elements[idx] != array_2.elements[idx])
            {
                equal = false;
                break;
            }
        }

        return equal;
    }

    template<typename TYPE, size_t SIZE>
    constexpr bool operator != (const Array<TYPE, SIZE>& array_1, const Array<TYPE, SIZE>& array_2) noexcept
    {
        auto equal = false;

        for(std::uint64_t idx = 0; idx < SIZE; ++idx)
        {
            if (array_1.elements[idx] != array_2.elements[idx])
            {
                equal = true;
                break;
            }
        }

        return equal;
    }

    template<typename TYPE, size_t SIZE>
    constexpr bool operator > (const Array<TYPE, SIZE>& array_1, const Array<TYPE, SIZE>& array_2) noexcept
    {
        for(std::uint64_t idx = 0; idx < SIZE; ++idx)
        {
            if (array_1.elements[idx] != array_2.elements[idx])
            {
                if (array_1.elements[idx] > array_2.elements[idx])
                {
                    return true;
                } else
                {
                    return false;
                }
            }
        }

        return false;
    }

    template<typename TYPE, size_t SIZE>
    constexpr bool operator >= (const Array<TYPE, SIZE>& array_1, const Array<TYPE, SIZE>& array_2) noexcept
    {
        for(std::uint64_t idx = 0; idx < SIZE; ++idx)
        {
            if (array_1.elements[idx] != array_2.elements[idx])
            {
                if (array_1.elements[idx] > array_2.elements[idx])
                {
                    return true;
                } else
                {
                    return false;
                }
            }
        }

        return true;
    }

    template<typename TYPE, size_t SIZE>
    constexpr bool operator < (const Array<TYPE, SIZE>& array_1, const Array<TYPE, SIZE>& array_2) noexcept
    {
        for(std::uint64_t idx = 0; idx < SIZE; ++idx)
        {
            if (array_1.elements[idx] != array_2.elements[idx])
            {
                if (array_1.elements[idx] < array_2.elements[idx])
                {
                    return true;
                } else
                {
                    return false;
                }
            }
        }

        return false;
    }

    template<typename TYPE, size_t SIZE>
    constexpr bool operator <= (const Array<TYPE, SIZE>& array_1, const Array<TYPE, SIZE>& array_2) noexcept
    {
        for(std::uint64_t idx = 0; idx < SIZE; ++idx)
        {
            if (array_1.elements[idx] != array_2.elements[idx])
            {
                if (array_1.elements[idx] < array_2.elements[idx])
                {
                    return true;
                } else
                {
                    return false;
                }
            }
        }

        return true;
    }

    template<typename TYPE>
    constexpr bool operator == (const Array<TYPE, 0>& array_1, const Array<TYPE, 0>& array_2) noexcept
    {
        return true;
    }

    template<typename TYPE>
    constexpr bool operator != (const Array<TYPE, 0>& array_1, const Array<TYPE, 0>& array_2) noexcept
    {
        return false;
    }

    template<typename TYPE>
    constexpr bool operator > (const Array<TYPE, 0>& array_1, const Array<TYPE, 0>& array_2) noexcept
    {
        return false;
    }

    template<typename TYPE>
    constexpr bool operator >= (const Array<TYPE, 0>& array_1, const Array<TYPE, 0>& array_2) noexcept
    {
        return true;
    }

    template<typename TYPE>
    constexpr bool operator < (const Array<TYPE, 0>& array_1, const Array<TYPE, 0>& array_2) noexcept
    {
        return false;
    }

    template<typename TYPE>
    constexpr bool operator <= (const Array<TYPE, 0>& array_1, const Array<TYPE, 0>& array_2) noexcept
    {
        return true;
    }

    template<typename TYPE, size_t SIZE_FIST, size_t SIZE_SECOND>
    constexpr Array<TYPE, SIZE_FIST + SIZE_SECOND> combining_arrays(const Array<TYPE, SIZE_FIST>& array_1, const Array<TYPE, SIZE_SECOND>& array_2)
    {
        Array<TYPE, SIZE_FIST + SIZE_SECOND> unionArray{};

        size_t unionArrayIndex = 0;

        for(size_t index = 0; index < SIZE_FIST; ++index)
        {
            unionArray.elements[unionArrayIndex] = array_1.elements[index];
            ++unionArrayIndex;
        }

        for(size_t index = 0; index < SIZE_SECOND; ++index)
        {
            unionArray.elements[unionArrayIndex] = array_2.elements[index];
            ++unionArrayIndex;
        }

        return unionArray;
    }

    template<typename TYPE>
    constexpr Array<TYPE, 0> combining_arrays(const Array<TYPE, 0>& array_1, const Array<TYPE, 0>& array_2)
    {
        return Array<TYPE, 0>{};
    }

    template<typename TYPE, size_t SIZE>
    constexpr Array<TYPE, SIZE> combining_arrays(const Array<TYPE, SIZE>& array_1, const Array<TYPE, 0>& array_2)
    {
        return Array<TYPE, SIZE>{array_1};
    }


}

#endif //INC_3DGAME_AUTO_ARRAY_H
