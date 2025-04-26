#ifndef INC_3DGAME_DYNAMIC_ARRAY_H
#define INC_3DGAME_DYNAMIC_ARRAY_H

#include <stdexcept>
#include <cstdlib>
#include <initializer_list>
#include <cassert>
#include <iostream>
#include <memory>
#include <iterator>
#include <utility>
#include <limits>

namespace bice
{

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    class VectorPOD
    {
    public:

        using value_type = TYPE;
        using size_type = std::uint64_t;
        using difference_type = std::int64_t;
        using allocator_type = ALLOCATOR;
        using reference = TYPE&;
        using const_reference = const TYPE&;
        using pointer = TYPE*;
        using const_pointer = const TYPE*;

        using iterator = TYPE*;
        using const_iterator = const TYPE*;

        explicit VectorPOD(allocator_type allocator_ = ALLOCATOR()) :
            allocator(allocator_),
            vectorSize{}, vectorCapacity{},
            beginVector{}, endVector{}
        {
        }

        explicit VectorPOD(const size_type count, allocator_type allocator_ = ALLOCATOR()) :
            allocator(allocator_),
            vectorSize(0), vectorCapacity(count),
            beginVector{},
            endVector{}
        {
            try
            {
                beginVector = allocator.allocate(count);
                endVector = beginVector + vectorSize;

            }
            catch (std::bad_alloc)
            {
                vectorSize = {};
                vectorCapacity = {};
                beginVector = {};
                endVector = {};

                return;
            }
        }

        VectorPOD(const size_type count, const_reference value, allocator_type allocator_ = ALLOCATOR()) :
            allocator(allocator_),
            vectorSize(count), vectorCapacity(count),
            beginVector{}, endVector{}
        {
            try
            {
                beginVector = allocator.allocate(count);
                endVector = beginVector + vectorSize;

            }
            catch (std::bad_alloc)
            {
                vectorSize = {};
                vectorCapacity = {};
                beginVector = {};
                endVector = {};

                return;
            }

            for (auto iter = beginVector; iter != endVector; ++iter)
            {
                *iter = value;
            }
        }

        VectorPOD(const std::initializer_list<value_type> list, allocator_type allocator_ = ALLOCATOR()) :
            allocator(allocator_),
            vectorSize(list.size()), vectorCapacity(list.size()),
            beginVector{}, endVector{}
        {
            try
            {
                beginVector = allocator.allocate(vectorCapacity);
                endVector = beginVector + vectorSize;
            }
            catch (std::bad_alloc)
            {
                vectorSize = {};
                vectorCapacity = {};
                beginVector = {};
                endVector = {};
                return;
            }

            auto iter = list.begin();

            for (auto it = beginVector; it != endVector; ++it)
            {
                *it = *iter;
                ++iter;
            }
        }

        template<typename ITERATOR>
        VectorPOD(const ITERATOR begin, const ITERATOR end, allocator_type allocator_ = ALLOCATOR()) :
            allocator(allocator_),
            vectorSize{}, vectorCapacity{},
            beginVector{}, endVector{}
        {

            size_type count = 0;

            for (auto iter = begin; iter != end; ++iter)
            {
                ++count;
            }

            try
            {
                beginVector = allocator.allocate(count);
            }
            catch (std::bad_alloc)
            {
                beginVector = {};
                return;
            }

            vectorSize = count;
            vectorCapacity = count;

            endVector = beginVector + vectorSize;

            pointer it = beginVector;

            for (auto iter = begin; iter != end; ++iter)
            {
                *it = *iter;
                ++it;
            }

        }

        VectorPOD(const VectorPOD<TYPE, ALLOCATOR>& other) :
            allocator(other.allocator),
            vectorSize(other.vectorSize),
            vectorCapacity(other.vectorCapacity),
            beginVector(allocator.allocate(vectorCapacity)),
            endVector(beginVector + vectorSize)
        {
            for (size_type index = 0; index < vectorSize; ++index)
            {
                beginVector[index] = other.beginVector[index];
            }
        }

        VectorPOD<TYPE, ALLOCATOR>& operator = (const VectorPOD<TYPE, ALLOCATOR>& other)
        {
            if (beginVector != nullptr && endVector != nullptr)
            {
                allocator.deallocate(beginVector, vectorCapacity);
            }

            allocator = other.allocator;
            vectorSize = other.vectorSize;
            vectorCapacity = other.vectorCapacity;

            beginVector = allocator.allocate(vectorCapacity);
            endVector = beginVector + vectorSize;

            for (size_type index = 0; index < vectorSize; ++index)
            {
                beginVector[index] = other.beginVector[index];
            }

            return *this;
        }

        VectorPOD(VectorPOD<TYPE, ALLOCATOR>&& other) noexcept :
            allocator(std::move(other.allocator)),
            vectorSize(std::move(other.vectorSize)),
            vectorCapacity(std::move(other.vectorCapacity)),
            beginVector(std::move(other.beginVector)),
            endVector(std::move(other.endVector))
        {
            other.vectorSize = {};
            other.vectorCapacity = {};
            other.beginVector = nullptr;
            other.endVector = nullptr;
        }

        VectorPOD<TYPE, ALLOCATOR>& operator = (VectorPOD<TYPE, ALLOCATOR>&& other)
        {
            if (beginVector != nullptr && endVector != nullptr)
            {
                allocator.deallocate(beginVector, vectorCapacity);
            }

            allocator = std::move(other.allocator);
            vectorSize = std::move(other.vectorSize);
            vectorCapacity = std::move(other.vectorCapacity);
            beginVector = std::move(other.beginVector);
            endVector = std::move(other.endVector);

            other.vectorSize = {};
            other.vectorCapacity = {};
            other.beginVector = nullptr;
            other.endVector = nullptr;

            return *this;
        }

        virtual ~VectorPOD()
        {
            if (beginVector != nullptr && endVector != nullptr)
            {
                allocator.deallocate(beginVector, vectorCapacity);
            }
            allocator.~ALLOCATOR();
        }

        [[nodiscard]] constexpr iterator begin() noexcept
        {
            return beginVector;
        }

        [[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
        {
            return reinterpret_cast<const_iterator>(beginVector);
        }

        [[nodiscard]] inline constexpr iterator end() noexcept
        {
            return endVector;
        }

        [[nodiscard]] inline constexpr const_iterator cend() const noexcept
        {
            return reinterpret_cast<const_iterator>(endVector);
        }

        [[nodiscard]] constexpr inline pointer begin_ptr() noexcept
        {
            return beginVector;
        }

        [[nodiscard]] constexpr inline pointer end_ptr() noexcept
        {
            return endVector;
        }

        [[nodiscard]] constexpr inline const_pointer cbegin_ptr() const noexcept
        {
            return reinterpret_cast<const_pointer>(beginVector);
        }

        [[nodiscard]] constexpr inline const_pointer cend_ptr() const noexcept
        {
            return reinterpret_cast<const_pointer>(endVector);
        }

        [[nodiscard]] inline size_type size() const noexcept
        {
            return vectorSize;
        }

        [[nodiscard]] inline size_type capacity() const noexcept
        {
            return vectorCapacity;
        }

        [[nodiscard]] inline size_type max_size() const noexcept
        {
            return std::numeric_limits<size_type>::max();
        }

        [[nodiscard]] inline constexpr allocator_type get_allocator() const noexcept
        {
            return allocator;
        }

        [[nodiscard]] inline constexpr pointer data() noexcept
        {
            return beginVector;
        }

        [[nodiscard]] inline constexpr const_pointer data() const noexcept
        {
            return reinterpret_cast<const_pointer>(beginVector);
        }

        void reserve(const size_type newCapacity)
        {
            if (newCapacity > vectorCapacity)
            {
                pointer newArray;

                try
                {
                    newArray = allocator.allocate(newCapacity);
                }
                catch (std::bad_alloc)
                {
                    beginVector = nullptr;
                    return;
                }

                auto dest = (void*)newArray;
                auto src = (void*)beginVector;
                size_type size = vectorSize * sizeof(TYPE);

                std::memcpy(dest, src, size);

                allocator.deallocate(beginVector, vectorCapacity);
                vectorCapacity = newCapacity;
                beginVector = newArray;
                endVector = beginVector + vectorSize;
            }
        }

        void shrink_to_fit()
        {
            if (vectorCapacity > vectorSize)
            {
                pointer newArray;

                try
                {
                    newArray = allocator.allocate(vectorSize);
                }
                catch (std::bad_alloc)
                {
                    return;
                }

                auto dest = (void*)newArray;
                auto src = (void*)beginVector;
                size_type size = vectorSize * sizeof(TYPE);

                std::memcpy(dest, src, size);

                allocator.deallocate(beginVector, vectorCapacity);
                vectorCapacity = vectorSize;
                beginVector = newArray;
                endVector = beginVector + vectorSize;
            }
        }

        void resize(const size_type count)
        {
            if (vectorCapacity == 0 && vectorSize == 0)
            {
                try
                {
                    beginVector = allocator.allocate(count);
                    endVector = beginVector + count;
                }
                catch (std::bad_alloc)
                {
                    beginVector = nullptr;
                    endVector = nullptr;
                    vectorSize = 0;
                    vectorCapacity = 0;
                    return;
                }

                vectorSize = count;
                vectorCapacity = count;

                for (size_type idx = 0; idx < vectorSize; ++idx)
                {
                    beginVector[idx] = {};
                }

            }
            else if (vectorCapacity > 0 && vectorSize == 0)
            {
                if (vectorCapacity >= count)
                {
                    vectorSize = count;
                    for (size_type idx = 0; idx < vectorSize; ++idx)
                    {
                        beginVector[idx] = {};
                    }
                    endVector = beginVector + vectorSize;
                }
                else if (vectorCapacity < count)
                {
                    try
                    {
                        allocator.deallocate(beginVector, vectorCapacity);
                        beginVector = allocator.allocate(count);
                        endVector = beginVector + count;
                    }
                    catch (std::bad_alloc)
                    {
                        beginVector = allocator.allocate(vectorCapacity);
                        endVector = beginVector + vectorSize;
                        return;
                    }

                    vectorSize = count;
                    vectorCapacity = count;
                    for (size_type idx = 0; idx < vectorSize; ++idx)
                    {
                        beginVector[idx] = {};
                    }

                }
            }
            else if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (count < vectorSize && count < vectorCapacity)
                {
                    vectorSize = count;
                    endVector = beginVector + vectorSize;
                }
                else if (count > vectorSize && count <= vectorCapacity)
                {
                    for (size_type idx = vectorSize; idx < count; ++idx)
                    {
                        beginVector[idx] = {};
                    }

                    vectorSize = count;
                    endVector = beginVector + vectorSize;
                }
                else if (count > vectorSize && count > vectorCapacity)
                {
                    pointer newArray{};

                    try
                    {
                        newArray = allocator.allocate(count);
                    }
                    catch (std::bad_alloc)
                    {
                        newArray = nullptr;
                        return;
                    }

                    /*for(size_type idx = 0; idx < vectorSize; ++idx)
                    {
                        newArray[idx] = beginVector[idx];
                    }*/

                    auto dest = (void*)newArray;
                    auto src = (void*)beginVector;
                    size_type size = vectorSize * sizeof(TYPE);

                    std::memcpy(dest, src, size);

                    allocator.deallocate(beginVector, vectorCapacity);
                    vectorCapacity = count;
                    beginVector = newArray;

                    for (size_type idx = vectorSize; idx < count; ++idx)
                    {
                        beginVector[idx] = {};
                    }
                    vectorSize = count;
                    endVector = beginVector + vectorSize;
                }
            }
        }

        void resize(const size_type count, const_reference value)
        {
            if (vectorCapacity == 0 && vectorSize == 0)
            {
                try
                {
                    beginVector = allocator.allocate(count);
                    endVector = beginVector + count;
                }
                catch (std::bad_alloc)
                {
                    beginVector = nullptr;
                    endVector = nullptr;
                    vectorSize = 0;
                    vectorCapacity = 0;
                    return;
                }

                vectorSize = count;
                vectorCapacity = count;

                for (size_type idx = 0; idx < vectorSize; ++idx)
                {
                    beginVector[idx] = value;
                }

            }
            else if (vectorCapacity > 0 && vectorSize == 0)
            {
                if (vectorCapacity >= count)
                {
                    vectorSize = count;
                    for (size_type idx = 0; idx < vectorSize; ++idx)
                    {
                        beginVector[idx] = value;
                    }
                    endVector = beginVector + vectorSize;
                }
                else if (vectorCapacity < count)
                {
                    try
                    {
                        allocator.deallocate(beginVector, vectorCapacity);
                        beginVector = allocator.allocate(count);
                        endVector = beginVector + count;
                    }
                    catch (std::bad_alloc)
                    {
                        beginVector = allocator.allocate(vectorCapacity);
                        endVector = beginVector + vectorSize;
                        return;
                    }

                    vectorSize = count;
                    vectorCapacity = count;
                    for (size_type idx = 0; idx < vectorSize; ++idx)
                    {
                        beginVector[idx] = value;
                    }

                }
            }
            else if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (count < vectorSize && count < vectorCapacity)
                {
                    vectorSize = count;
                    endVector = beginVector + vectorSize;
                }
                else if (count > vectorSize && count <= vectorCapacity)
                {
                    for (size_type idx = vectorSize; idx < count; ++idx)
                    {
                        beginVector[idx] = value;
                    }

                    vectorSize = count;
                    endVector = beginVector + vectorSize;
                }
                else if (count > vectorSize && count > vectorCapacity)
                {
                    pointer newArray{};

                    try
                    {
                        newArray = allocator.allocate(count);
                    }
                    catch (std::bad_alloc)
                    {
                        newArray = nullptr;
                        return;
                    }

                    /*for(size_type idx = 0; idx < vectorSize; ++idx)
                    {
                        newArray[idx] = beginVector[idx];
                    }*/

                    auto dest = (void*)newArray;
                    auto src = (void*)beginVector;
                    size_type size = vectorSize * sizeof(TYPE);

                    std::memcpy(dest, src, size);

                    allocator.deallocate(beginVector, vectorCapacity);
                    vectorCapacity = count;
                    beginVector = newArray;

                    for (size_type idx = vectorSize; idx < count; ++idx)
                    {
                        beginVector[idx] = value;
                    }
                    vectorSize = count;
                    endVector = beginVector + vectorSize;
                }
            }
        }

        inline void clear()
        {
            vectorSize = 0;
        }

        [[nodiscard]] inline constexpr reference front()
        {
            if (vectorSize > 0)
            {
                return beginVector[0];
            }
            else
            {
                throw std::out_of_range("Vector is empty");
            }
        }

        [[nodiscard]] inline constexpr const_reference front() const
        {
            if (vectorSize > 0)
            {
                return reinterpret_cast<const_reference>(beginVector[0]);
            }
            else
            {
                throw std::out_of_range("Vector is empty");
            }

        }

        [[nodiscard]] inline constexpr reference back()
        {
            if (vectorSize > 0)
            {
                return beginVector[vectorSize - 1];
            }
            else
            {
                throw std::out_of_range("Vector is empty");
            }
        }

        [[nodiscard]] inline constexpr const_reference back() const
        {
            if (vectorSize > 0)
            {
                return reinterpret_cast<const_reference>(beginVector[vectorSize - 1]);
            }
            else
            {
                throw std::out_of_range("Vector is empty");
            }
        }

        void push_back(const_reference value)
        {
            if (vectorCapacity == 0 && vectorSize == 0)
            {
                try
                {
                    beginVector = allocator.allocate(1);
                }
                catch (std::bad_alloc)
                {
                    beginVector = nullptr;
                    endVector = nullptr;
                    vectorSize = {};
                    vectorCapacity = {};
                    return;
                }

                vectorSize = 1;
                vectorCapacity = 1;
                beginVector[0] = value;
                endVector = beginVector + 1;

            }
            else if (vectorCapacity > 0)
            {
                if (vectorSize == vectorCapacity)
                {
                    if (vectorCapacity == 1)
                    {
                        reserve(2);
                    }
                    else
                    {
                        reserve(vectorCapacity * 1.5);
                    }
                }

                beginVector[vectorSize++] = value;
                ++endVector;
            }

        }

        void push_back(value_type&& value)
        {
            if (vectorCapacity == 0 && vectorSize == 0)
            {
                try
                {
                    beginVector = allocator.allocate(1);
                }
                catch (std::bad_alloc)
                {
                    beginVector = nullptr;
                    endVector = nullptr;
                    vectorSize = {};
                    vectorCapacity = {};
                    return;
                }

                vectorSize = 1;
                vectorCapacity = 1;
                beginVector[0] = std::move(value);
                endVector = beginVector + 1;

            }
            else if (vectorCapacity > 0)
            {
                if (vectorSize == vectorCapacity)
                {
                    if (vectorCapacity == 1)
                    {
                        reserve(2);
                    }
                    else
                    {
                        reserve(vectorCapacity * 1.5);
                    }
                }

                beginVector[vectorSize++] = std::move(value);
                ++endVector;
            }
        }

        void pop_back()
        {
            if (vectorSize > 0)
            {
                --vectorSize;
                if (vectorSize == 0)
                    --endVector;
            }
        }

        void assign(const size_type count, const_reference value)
        {
            if (count >= 0)
            {
                if (vectorCapacity > 0)
                {
                    allocator.deallocate(beginVector, vectorCapacity);
                    beginVector = nullptr;
                    endVector = nullptr;
                    vectorSize = 0;
                    vectorCapacity = 0;
                }

                if (count > 0)
                {
                    try
                    {
                        beginVector = allocator.allocate(count);
                        endVector = beginVector + count;
                    }
                    catch (std::bad_alloc)
                    {
                        beginVector = nullptr;
                        endVector = nullptr;
                        return;
                    }

                    vectorCapacity = count;
                    vectorSize = count;

                    for (size_type idx = 0; idx < vectorSize; ++idx)
                    {
                        beginVector[idx] = value;
                    }
                }

            }
            else
            {
                throw std::out_of_range("Count cannot be negative");
            }

        }

        template<typename InputIt>
        void assign(InputIt first, InputIt last)
        {
            //const difference_type length = last - first;
            difference_type length;

            for (auto iter = first; iter != last; ++iter)
            {
                ++length;
            }
            if (length >= 0)
            {
                if (vectorCapacity > 0)
                {
                    allocator.deallocate(beginVector, vectorCapacity);
                    beginVector = nullptr;
                    endVector = nullptr;
                    vectorSize = 0;
                    vectorCapacity = 0;
                }

                if (length > 0)
                {
                    try
                    {
                        beginVector = allocator.allocate(length);
                        endVector = beginVector + length;
                    }
                    catch (std::bad_alloc)
                    {
                        beginVector = nullptr;
                        endVector = nullptr;
                    }

                    vectorCapacity = length;
                    vectorSize = length;

                    auto iter = beginVector;

                    for (auto it = first; it != last; ++it)
                    {
                        *iter = *it;
                        ++iter;
                    }
                }
            }
            else
            {
                throw std::out_of_range("The distance between iterators cannot be negative");
            }
        }

        void insert_idx(const size_type position, const_reference value)
        {
            if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (position >= 0 && position < vectorSize)
                {
                    if (vectorSize == vectorCapacity)
                    {
                        if (vectorCapacity == 1)
                        {
                            reserve(2);
                        }
                        else
                        {
                            reserve(vectorCapacity * 1.5);
                        }
                    }

                    for (size_type idx = vectorSize; idx > position; --idx)
                    {
                        beginVector[idx] = beginVector[idx - 1];
                    }

                    beginVector[position] = value;
                    ++vectorSize;
                    ++endVector;

                }
                else
                {
                    throw std::out_of_range("Index out bound");
                }

            }
            else
            {
                throw std::out_of_range("Vector cannot be empty and must contain at least one element");
            }
        }

        void insert_idx(const size_type position, value_type&& value)
        {
            if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (position >= 0 && position < vectorSize)
                {
                    if (vectorSize == vectorCapacity)
                    {
                        if (vectorCapacity == 1)
                        {
                            reserve(2);
                        }
                        else
                        {
                            reserve(vectorCapacity * 1.5);
                        }
                    }

                    for (size_type idx = vectorSize; idx > position; --idx)
                    {
                        beginVector[idx] = beginVector[idx - 1];
                    }

                    beginVector[position] = std::move(value);
                    ++vectorSize;
                    ++endVector;

                }
                else
                {
                    throw std::out_of_range("Index out bound");
                }

            }
            else
            {
                throw std::out_of_range("Vector cannot be empty and must contain at least one element");
            }
        }

        void insert_idx(const size_type position, const size_type count, const_reference value)
        {
            if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (count > 0)
                {
                    if (position >= 0 && position < vectorSize)
                    {
                        if (vectorSize == vectorCapacity)
                        {
                            reserve(vectorCapacity + count + 1);
                        }

                        vectorSize += count;

                        for (size_type idx = vectorSize; idx > position; --idx)
                        {
                            beginVector[idx] = beginVector[idx - count];
                        }

                        const size_type endPosition = position + count;

                        for (size_type idx = position; idx < endPosition; ++idx)
                        {
                            beginVector[idx] = value;
                        }

                        endVector = beginVector + vectorSize;

                    }
                    else
                    {
                        throw std::out_of_range("Index out bound");
                    }

                }
                else
                {
                    throw std::out_of_range("Count must be greater than 0 ");
                }

            }
            else
            {
                throw std::out_of_range("Vector cannot be empty and must contain at least one element");
            }
        }

        template<typename ITERATOR>
        void insert_idx_itr(const size_type position, const ITERATOR first, const ITERATOR last)
        {
            const difference_type length = last - first;

            if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (length > 0)
                {
                    if (position >= 0 && position < vectorSize)
                    {
                        if (vectorSize == vectorCapacity)
                        {
                            reserve(vectorCapacity + length + 1);
                        }

                        vectorSize += length;

                        for (size_type idx = vectorSize; idx > position; --idx)
                        {
                            beginVector[idx] = beginVector[idx - length];
                        }

                        const size_type endPosition = position + length;

                        auto iter = first;

                        for (size_type idx = position; idx < endPosition; ++idx)
                        {
                            beginVector[idx] = *iter;
                            ++iter;
                        }

                        endVector = beginVector + vectorSize;

                    }
                    else
                    {
                        throw std::out_of_range("Index out bound");
                    }

                }
                else
                {
                    throw std::out_of_range("The distance between iterators cannot be negative or 0 ");
                }

            }
            else
            {
                throw std::out_of_range("Vector cannot be empty and must contain at least one element");
            }

        }

        void insert_idx(const size_type position, std::initializer_list<value_type> list)
        {
            const difference_type length = list.size();

            if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (length > 0)
                {
                    if (position >= 0 && position < vectorSize)
                    {
                        if (vectorSize == vectorCapacity)
                        {
                            reserve(vectorCapacity + length + 1);
                        }

                        vectorSize += length;

                        for (size_type idx = vectorSize; idx > position; --idx)
                        {
                            beginVector[idx] = beginVector[idx - length];
                        }

                        const size_type endPosition = position + length;

                        auto iter = list.begin();

                        for (size_type idx = position; idx < endPosition; ++idx)
                        {
                            beginVector[idx] = *iter;
                            ++iter;
                        }

                        endVector = beginVector + vectorSize;

                    }
                    else
                    {
                        throw std::out_of_range("Index out bound");
                    }

                }
                else
                {
                    throw std::out_of_range("The initializer list cannot be negative or 0 ");
                }

            }
            else
            {
                throw std::out_of_range("Vector cannot be empty and must contain at least one element");
            }
        }

        iterator insert(const_iterator position, const_reference value)
        {
            const auto insertIndex = (size_type)(position - beginVector);

            if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (insertIndex >= 0 && insertIndex < vectorSize)
                {
                    if (vectorSize == vectorCapacity)
                    {
                        if (vectorCapacity == 1)
                        {
                            reserve(2);
                        }
                        else
                        {
                            reserve(vectorCapacity * 1.5);
                        }
                    }

                    for (size_type idx = vectorSize; idx > insertIndex; --idx)
                    {
                        beginVector[idx] = beginVector[idx - 1];
                    }

                    beginVector[insertIndex] = value;
                    ++vectorSize;
                    ++endVector;

                    return &beginVector[insertIndex];
                }
                else
                {
                    throw std::out_of_range("Index out bound");
                }

            }
            else
            {
                throw std::out_of_range("Vector cannot be empty and must contain at least one element");
            }
        }

        iterator insert(const_iterator position, value_type&& value)
        {
            const auto insertIndex = (size_type)(position - beginVector);

            if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (insertIndex >= 0 && insertIndex < vectorSize)
                {
                    if (vectorSize == vectorCapacity)
                    {
                        if (vectorCapacity == 1)
                        {
                            reserve(2);
                        }
                        else
                        {
                            reserve(vectorCapacity * 1.5);
                        }
                    }

                    for (size_type idx = vectorSize; idx > insertIndex; --idx)
                    {
                        beginVector[idx] = beginVector[idx - 1];
                    }

                    beginVector[insertIndex] = std::move(value);
                    ++vectorSize;
                    ++endVector;

                    return &beginVector[insertIndex];
                }
                else
                {
                    throw std::out_of_range("Index out bound");
                }

            }
            else
            {
                throw std::out_of_range("Vector cannot be empty and must contain at least one element");
            }
        }

        iterator insert(const_iterator position, const size_type count, const_reference value)
        {
            const auto insertIndex = position - beginVector;

            if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (count > 0)
                {
                    if (insertIndex >= 0 && insertIndex < vectorSize)
                    {
                        if (vectorSize == vectorCapacity)
                        {
                            reserve(vectorCapacity + count + 1);
                        }

                        vectorSize += count;

                        for (size_type idx = vectorSize; idx > insertIndex; --idx)
                        {
                            beginVector[idx] = beginVector[idx - count];
                        }

                        const size_type endPosition = insertIndex + count;

                        for (size_type idx = insertIndex; idx < endPosition; ++idx)
                        {
                            beginVector[idx] = value;
                        }

                        endVector = beginVector + vectorSize;

                        return &beginVector[insertIndex];

                    }
                    else
                    {
                        throw std::out_of_range("Index out bound");
                    }

                }
                else
                {
                    throw std::out_of_range("Count must be greater than 0 ");
                }

            }
            else
            {
                throw std::out_of_range("Vector cannot be empty and must contain at least one element");
            }
        }

        template<typename ITERATOR>
        iterator insert(const_iterator position, ITERATOR first, ITERATOR last)
        {
            const auto insertIndex = position - beginVector;
            //difference_type length = last - first;

            difference_type length;

            for (auto iter = first; iter != last; ++iter)
            {
                ++length;
            }

            if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (length > 0)
                {
                    if (insertIndex >= 0 && insertIndex < vectorSize)
                    {
                        if (vectorSize == vectorCapacity)
                        {
                            reserve(vectorCapacity + length + 1);
                        }

                        vectorSize += length;

                        for (size_type idx = vectorSize; idx > insertIndex; --idx)
                        {
                            beginVector[idx] = beginVector[idx - length];
                        }

                        const size_type endPosition = insertIndex + length;

                        auto iter = first;

                        for (size_type idx = insertIndex; idx < endPosition; ++idx)
                        {
                            beginVector[idx] = *iter;
                            ++iter;
                        }

                        endVector = beginVector + vectorSize;

                        return &beginVector[insertIndex];

                    }
                    else
                    {
                        throw std::out_of_range("Index out bound");
                    }

                }
                else
                {
                    throw std::out_of_range("The distance between iterators cannot be negative or 0 ");
                }

            }
            else
            {
                throw std::out_of_range("Vector cannot be empty and must contain at least one element");
            }

        }

        iterator insert(const_iterator position, std::initializer_list<value_type> list)
        {
            const auto insertIndex = position - beginVector;

            const difference_type length = list.size();

            if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (length > 0)
                {
                    if (insertIndex >= 0 && insertIndex < vectorSize)
                    {
                        if (vectorSize == vectorCapacity)
                        {
                            reserve(vectorCapacity + length + 1);
                        }

                        vectorSize += length;

                        for (size_type idx = vectorSize; idx > insertIndex; --idx)
                        {
                            beginVector[idx] = beginVector[idx - length];
                        }

                        const size_type endPosition = insertIndex + length;

                        auto iter = list.begin();

                        for (size_type idx = insertIndex; idx < endPosition; ++idx)
                        {
                            beginVector[idx] = *iter;
                            ++iter;
                        }

                        endVector = beginVector + vectorSize;

                        return &beginVector[insertIndex];

                    }
                    else
                    {
                        throw std::out_of_range("Index out bound");
                    }

                }
                else
                {
                    throw std::out_of_range("The initializer list cannot be negative or 0 ");
                }

            }
            else
            {
                throw std::out_of_range("Vector cannot be empty and must contain at least one element");
            }
        }

        void erase(const size_type position)
        {
            if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (position >= 0 && position < vectorSize)
                {
                    for (size_type idx = position; idx < vectorSize - 1; ++idx)
                    {
                        beginVector[idx] = beginVector[idx + 1];
                    }
                    --vectorSize;
                    --endVector;

                }
                else
                {
                    throw std::out_of_range("Index out bound");
                }

            }
            else
            {
                throw std::out_of_range("Vector cannot be empty and must contain at least one element");
            }
        }

        void erase(const size_type beginIndex, const size_type endIndex)
        {
            const size_type length = endIndex - beginIndex;

            if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (length >= 0 && length <= vectorSize)
                {
                    if (beginIndex >= 0 && beginIndex <= vectorSize && endIndex >= 0 && endIndex <= vectorSize)
                    {
                        for (size_type idx = beginIndex; idx < vectorSize - length; ++idx)
                        {
                            beginVector[idx] = beginVector[idx + length];
                        }

                        vectorSize -= length;
                        endVector -= length;

                    }
                    else
                    {
                        throw std::out_of_range("Indices out of bound");
                    }

                }
                else
                {
                    throw std::out_of_range("The distance must be greater than 0, but not greater than the length of the vector");
                }

            }
            else
            {
                throw std::out_of_range("Vector cannot be empty and must contain at least one element");
            }
        }

        iterator erase(const_iterator position)
        {
            const auto eraseIndex = position - beginVector;

            if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (eraseIndex >= 0 && eraseIndex < vectorSize)
                {
                    for (size_type idx = eraseIndex; idx < vectorSize - 1; ++idx)
                    {
                        beginVector[idx] = beginVector[idx + 1];
                    }
                    --vectorSize;
                    --endVector;

                    return &beginVector[eraseIndex];

                }
                else
                {
                    throw std::out_of_range("Index out bound");
                }

            }
            else
            {
                throw std::out_of_range("Vector cannot be empty and must contain at least one element");
            }
        }

        iterator erase(const_iterator first, const_iterator last)
        {
            const auto length = (size_type)(last - first);
            const size_type beginIndex = first - beginVector;

            if (vectorSize > 0 && vectorCapacity > 0)
            {
                if (length >= 0 && length <= vectorSize)
                {
                    if (first >= beginVector && first <= endVector && last >= beginVector && last <= endVector)
                    {
                        for (size_type idx = beginIndex; idx < vectorSize - length; ++idx)
                        {
                            beginVector[idx] = beginVector[idx + length];
                        }

                        vectorSize -= length;
                        endVector -= length;

                        return &beginVector[beginIndex];

                    }
                    else
                    {
                        throw std::out_of_range("Iterators out of bound");
                    }

                }
                else
                {
                    throw std::out_of_range("The distance must be greater than 0, but not greater than the length of the vector");
                }

            }
            else
            {
                throw std::out_of_range("Vector cannot be empty and must contain at least one element");
            }
        }

        [[nodiscard]] inline reference at(const size_type index)
        {
            if (index >= 0 && index < vectorSize)
            {
                return beginVector[index];
            }
            else
            {
                throw std::out_of_range("Index out of bound");
            }
        }

        [[nodiscard]] inline const_reference at(const size_type index) const
        {
            if (index >= 0 && index < vectorSize)
            {
                return reinterpret_cast<const_reference>(beginVector[index]);
            }
            else
            {
                throw std::out_of_range("Index out of bound");
            }
        }

        [[nodiscard]] inline reference operator [] (const size_type index) noexcept
        {
            return beginVector[index];
        }

        [[nodiscard]] inline const_reference operator [] (const size_type index) const noexcept
        {
            return reinterpret_cast<const_reference>(beginVector[index]);
        }

        void swap(VectorPOD<TYPE, ALLOCATOR>& other)
        {
            auto tempSize = vectorSize;
            auto tempCapacity = vectorCapacity;
            auto tempBegin = beginVector;
            auto tempEnd = endVector;

            vectorSize = other.vectorSize;
            vectorCapacity = other.vectorCapacity;
            beginVector = other.beginVector;
            endVector = other.endVector;

            other.vectorSize = tempSize;
            other.vectorCapacity = tempCapacity;
            other.beginVector = tempBegin;
            other.endVector = tempEnd;
        }

        void print() const noexcept
        {
            std::cout << "VectorPOD: size = " << vectorSize << ", stackCapacity = " << vectorCapacity << std::endl;
            for (auto iter = beginVector; iter != endVector; ++iter)
            {
                std::cout << *iter << ", ";
            }
            std::cout << std::endl;
        }

    private:

        allocator_type allocator;

        size_type vectorSize;
        size_type vectorCapacity;
        pointer beginVector;
        pointer endVector;
    };

#define vector_pod VectorPOD<TYPE, ALLOCATOR>
#define cvector_pod const VectorPOD<TYPE, ALLOCATOR>

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    bool operator == (cvector_pod& left, cvector_pod& right)
    {
        bool equal = true;

        if (left.size() == right.size())
        {
            for (std::uint64_t idx = 0; idx < left.size(); ++idx)
            {
                if (left[idx] != right[idx])
                {
                    equal = false;
                    break;
                }
            }

        }
        else
        {
            equal = false;
        }

        return equal;
    }

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    bool operator != (cvector_pod& left, cvector_pod& right)
    {
        bool equal = false;

        if (left.size() == right.size())
        {
            for (std::uint64_t idx = 0; idx < left.size(); ++idx)
            {
                if (left[idx] != right[idx])
                {
                    equal = true;
                    break;
                }
            }
        }
        else
        {
            equal = true;
        }

        return equal;
    }

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    bool operator >= (cvector_pod& left, cvector_pod& right)
    {
        bool equal;

        if (left.size() == right.size())
        {
            equal = true;
            for (std::uint64_t idx = 0; idx < left.size(); ++idx)
            {
                if (left[idx] != right[idx])
                {
                    if (left[idx] > right[idx])
                    {
                        break;
                    }
                    else
                    {
                        equal = false;
                        break;
                    }
                }
            }
        }
        else if (left.size() > right.size())
        {
            equal = true;
            for (std::uint64_t idx = 0; idx < right.size(); ++idx)
            {
                if (left[idx] != right[idx])
                {
                    if (left[idx] > right[idx])
                    {
                        break;
                    }
                    else
                    {
                        equal = false;
                        break;
                    }
                }
            }
        }
        else
        {
            equal = false;
            for (std::uint64_t idx = 0; idx < left.size(); ++idx)
            {
                if (left[idx] != right[idx])
                {
                    if (left[idx] > right[idx])
                    {
                        equal = true;
                        break;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        return equal;
    }

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    bool operator > (cvector_pod& left, cvector_pod& right)
    {
        bool equal;

        if (left.size() == right.size())
        {
            equal = false;
            for (std::uint64_t idx = 0; idx < left.size(); ++idx)
            {
                if (left[idx] != right[idx])
                {
                    if (left[idx] > right[idx])
                    {
                        equal = true;
                        break;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else if (left.size() > right.size())
        {
            equal = true;
            for (std::uint64_t idx = 0; idx < right.size(); ++idx)
            {
                if (left[idx] != right[idx])
                {
                    if (left[idx] > right[idx])
                    {
                        break;
                    }
                    else
                    {
                        equal = false;
                        break;
                    }
                }
            }
        }
        else
        {
            equal = false;
            for (std::uint64_t idx = 0; idx < left.size(); ++idx)
            {
                if (left[idx] != right[idx])
                {
                    if (left[idx] > right[idx])
                    {
                        equal = true;
                        break;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        return equal;
    }

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    bool operator <= (cvector_pod& left, cvector_pod& right)
    {
        bool equal;

        if (left.size() == right.size())
        {
            equal = true;
            for (std::uint64_t idx = 0; idx < left.size(); ++idx)
            {
                if (left[idx] != right[idx])
                {
                    if (left[idx] < right[idx])
                    {
                        break;
                    }
                    else
                    {
                        equal = false;
                        break;
                    }
                }
            }
        }
        else if (left.size() > right.size())
        {
            equal = false;
            for (std::uint64_t idx = 0; idx < right.size(); ++idx)
            {
                if (left[idx] != right[idx])
                {
                    if (left[idx] < right[idx])
                    {
                        equal = true;
                        break;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            equal = true;
            for (std::uint64_t idx = 0; idx < left.size(); ++idx)
            {
                if (left[idx] != right[idx])
                {
                    if (left[idx] < right[idx])
                    {
                        break;
                    }
                    else
                    {
                        equal = false;
                        break;
                    }
                }
            }
        }

        return equal;
    }

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    bool operator < (cvector_pod& left, cvector_pod& right)
    {
        bool equal;

        if (left.size() == right.size())
        {
            equal = false;
            for (std::uint64_t idx = 0; idx < left.size(); ++idx)
            {
                if (left[idx] != right[idx])
                {
                    if (left[idx] < right[idx])
                    {
                        equal = true;
                        break;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else if (left.size() > right.size())
        {
            equal = false;
            for (std::uint64_t idx = 0; idx < right.size(); ++idx)
            {
                if (left[idx] != right[idx])
                {
                    if (left[idx] < right[idx])
                    {
                        equal = true;
                        break;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            equal = true;
            for (std::uint64_t idx = 0; idx < left.size(); ++idx)
            {
                if (left[idx] != right[idx])
                {
                    if (left[idx] < right[idx])
                    {
                        break;
                    }
                    else
                    {
                        equal = false;
                        break;
                    }
                }
            }
        }


        return equal;
    }

    
}

#endif //INC_3DGAME_DYNAMIC_ARRAY_H
