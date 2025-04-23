#ifndef INC_3DGAME_DEQUE_VECTOR_H
#define INC_3DGAME_DEQUE_VECTOR_H

#include <stdexcept>
#include <cstdlib>
#include <cstdint>
#include <memory>

namespace bice
{
    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    class DequeVector
    {
    public:

        using value_type                   = TYPE;
        using size_type                    = std::uint64_t;
        using difference_type              = std::int64_t;
        using allocator_type               = ALLOCATOR;
        using reference                    = TYPE&;
        using const_reference              = const TYPE&;
        using pointer                      = TYPE*;
        using const_pointer                = const TYPE*;

        DequeVector(allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        dequeSize{},
        beginArray{}, endArray{},
        frontPointer{}, backPointer{}
        {}

        explicit DequeVector(const size_type size, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        dequeSize{},
        beginArray{}, endArray{},
        frontPointer{}, backPointer{}
        {
            if (size > 0)
            {
                pointer workArray{nullptr};
                pointer workIterator{nullptr};

                try
                {
                    workArray = allocator.allocate(size);
                    workIterator = workArray;

                    for(size_type idx = 0; idx < size; ++idx)
                    {
                        new(workIterator) value_type();
                        ++workIterator;
                    }

                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    for(auto iter = workArray; iter != workIterator; ++iter)
                    {
                        (*iter).~value_type();
                    }
                    allocator.deallocate(workArray, size);
                    return;
                }

                dequeSize = size;
                beginArray = workArray;
                endArray = beginArray + size;
                frontPointer = beginArray;
                backPointer = endArray - 1;
            }
        }

        explicit DequeVector(const size_type size, const_reference value, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        dequeSize{},
        beginArray{}, endArray{},
        frontPointer{}, backPointer{}
        {
            if (size > 0)
            {
                pointer workArray{nullptr};
                pointer workIterator{nullptr};

                try
                {
                    workArray = allocator.allocate(size);
                    workIterator = workArray;

                    for(size_type idx = 0; idx < size; ++idx)
                    {
                        new(workIterator) value_type(value);
                        ++workIterator;
                    }

                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    for(auto iter = workArray; iter != workIterator; ++iter)
                    {
                        (*iter).~value_type();
                    }
                    allocator.deallocate(workArray, size);
                    return;
                }

                dequeSize = size;
                beginArray = workArray;
                endArray = beginArray + size;
                frontPointer = beginArray;
                backPointer = endArray - 1;
            }
        }

        DequeVector(std::initializer_list<value_type> list, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        dequeSize{},
        beginArray{}, endArray{},
        frontPointer{}, backPointer{}
        {
            if (list.size())
            {
                pointer workArray{nullptr};
                pointer workIterator{nullptr};

                try
                {
                    workArray = allocator.allocate(list.size());
                    workIterator = workArray;

                    for(auto iter = list.begin(); iter != list.end(); ++iter)
                    {
                        new(workIterator) value_type(*iter);
                        ++workIterator;
                    }
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    for(auto iter = workArray; iter != workIterator; ++iter)
                    {
                        (*iter).~value_type();
                    }
                    allocator.deallocate(workArray, list.size());
                    return;
                }
                dequeSize = list.size();
                beginArray = workArray;
                endArray = beginArray + dequeSize;
                frontPointer = beginArray;
                backPointer = endArray - 1;
            }
        }

        template<typename ITERATOR>
        DequeVector(const ITERATOR begin, const ITERATOR end, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        dequeSize{},
        beginArray{}, endArray{},
        frontPointer{}, backPointer{}
        {
            difference_type distance{};
            for(auto iter = begin; iter != end; ++iter)
            {
                ++distance;
            }

            if (distance > 0)
            {
                pointer workArray{nullptr};
                pointer workIterator{nullptr};

                try
                {
                    workArray = allocator.allocate(distance);
                    workIterator = workArray;

                    for(auto iter = begin; iter != end; ++iter)
                    {
                        new(workIterator) value_type(*iter);
                        ++workIterator;
                    }
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    for(auto iter = workArray; iter != workIterator; ++iter)
                    {
                        (*iter).~value_type();
                    }
                    allocator.deallocate(workArray, distance);
                    return;
                }
                dequeSize = distance;
                beginArray = workArray;
                endArray = beginArray + dequeSize;
                frontPointer = beginArray;
                backPointer = endArray - 1;
            }
        }

        DequeVector(const DequeVector<TYPE, ALLOCATOR>& other)
        {
            dequeSize = other.dequeSize;
            beginArray = nullptr;
            endArray = nullptr;
            frontPointer = nullptr;
            backPointer = nullptr;
            if (dequeSize > 0)
            {
                pointer workArray{nullptr};
                pointer workIterator{nullptr};

                try
                {
                    workArray = allocator.allocate(dequeSize);
                    workIterator = workArray;

                    pointer begin = other.frontPointer;
                    pointer end = other.backPointer + 1;

                    for(auto iter = begin; iter != end; ++iter)
                    {
                        new(workIterator) value_type(*iter);
                        ++workIterator;
                    }

                }catch (std::bad_alloc)
                {
                    dequeSize = 0;
                    return;
                } catch (...)
                {
                    for(auto iter = workArray; iter != workIterator; ++iter)
                    {
                        (*iter).~value_type();
                    }
                    allocator.deallocate(workArray, dequeSize);
                    dequeSize = 0;
                    return;
                }
                beginArray = workArray;
                endArray = beginArray + dequeSize;
                frontPointer = beginArray;
                backPointer = endArray - 1;
            }
        }

        DequeVector<TYPE, ALLOCATOR>& operator = (const DequeVector<TYPE, ALLOCATOR>& other)
        {
            if (dequeSize == other.dequeSize && dequeSize > 0)
            {
                pointer worIterator = frontPointer;
                const pointer begin = other.frontPointer;
                const pointer end = other.backPointer + 1;

                for(auto iter = begin; iter != end; ++iter)
                {
                    *worIterator = *iter;
                }
            } else if (other.dequeSize <= (endArray - beginArray))
            {
                const pointer begin = frontPointer;
                const pointer end = backPointer + 1;
                for(auto iter = begin; iter != end; ++iter)
                {
                    (*iter).~value_type();
                }

                pointer workIterator = beginArray;

                try
                {
                    for(size_type idx = 0; idx < other.dequeSize; ++idx)
                    {
                        new(workIterator) value_type(other.frontPointer[idx]);
                        ++workIterator;
                    }
                } catch (...)
                {
                    for(auto iter = beginArray; iter != workIterator; ++iter)
                    {
                        (*iter).~value_type();
                    }
                    dequeSize = 0;
                    frontPointer = beginArray;
                    backPointer = beginArray;
                    return *this;
                }
                dequeSize = other.dequeSize;
                frontPointer = beginArray;
                backPointer = beginArray + dequeSize - 1;
            } else
            {
                if (dequeSize > 0)
                {
                    const pointer begin = frontPointer;
                    const pointer end = backPointer + 1;
                    for(auto iter = begin; iter != end; ++iter)
                    {
                        (*iter).~value_type();
                    }
                }

                if (beginArray != nullptr)
                {
                    const size_type capacity = endArray - beginArray;
                    allocator.deallocate(beginArray, capacity);
                }

                dequeSize = other.dequeSize;
                beginArray = nullptr;
                endArray = nullptr;
                frontPointer = nullptr;
                backPointer = nullptr;

                if (dequeSize > 0)
                {
                    pointer workArray{nullptr};
                    pointer workIterator{nullptr};

                    try
                    {
                        workArray = allocator.allocate(dequeSize);
                        workIterator = workArray;
                        const pointer begin = other.frontPointer;
                        const pointer end = other.backPointer + 1;

                        for(auto iter = begin; iter != end; ++iter)
                        {
                            new(workIterator) value_type(*iter);
                            ++workIterator;
                        }
                    } catch (std::bad_alloc)
                    {
                        dequeSize = 0;
                        return *this;
                    } catch (...)
                    {
                        for(auto iter = workArray; iter != workIterator; ++iter)
                        {
                            (*iter).~value_type();
                        }
                        allocator.deallocate(workArray, dequeSize);
                        dequeSize = 0;
                        return *this;
                    }

                    beginArray = workArray;
                    endArray = beginArray + dequeSize;
                    frontPointer = beginArray;
                    backPointer = endArray - 1;
                }
            }
            return *this;
        }

        DequeVector(DequeVector<TYPE, ALLOCATOR>&& other) noexcept :
        dequeSize(std::move(other.dequeSize)),
        beginArray(std::move(other.beginArray)),
        endArray(std::move(other.endArray)),
        frontPointer(std::move(other.frontPointer)),
        backPointer(std::move(other.backPointer))
        {
            other.dequeSize = 0;
            other.beginArray = nullptr;
            other.endArray = nullptr;
            other.frontPointer = nullptr;
            other.backPointer = nullptr;
        }

        DequeVector<TYPE, ALLOCATOR>& operator = (DequeVector<TYPE, ALLOCATOR>&& other) noexcept
        {
            if (dequeSize > 0)
            {
                const pointer begin = frontPointer;
                const pointer end = backPointer + 1;
                for(auto iter = begin; iter != end; ++iter)
                {
                    (*iter).~value_type();
                }
            }

            if (beginArray != nullptr)
            {
                const size_type capacity = endArray - beginArray;
                allocator.deallocate(beginArray, capacity);
            }

            dequeSize = std::move(other.dequeSize);
            beginArray = std::move(other.beginArray);
            endArray = std::move(other.endArray);
            frontPointer = std::move(other.frontPointer);
            backPointer = std::move(other.backPointer);

            other.dequeSize = 0;
            other.beginArray = nullptr;
            other.endArray = nullptr;
            other.frontPointer = nullptr;
            other.backPointer = nullptr;

            return *this;
        }

        virtual ~DequeVector()
        {
            if (dequeSize > 0)
            {
                const pointer begin = frontPointer;
                const pointer end = backPointer + 1;
                for(auto iter = begin; iter != end; ++iter)
                {
                    (*iter).~value_type();
                }
            }

            if (beginArray != nullptr)
            {
                const size_type capacity = endArray - beginArray;
                allocator.deallocate(beginArray, capacity);
            }
        }

        [[nodiscard]] inline size_type size() const noexcept
        {
            return dequeSize;
        }

        [[nodiscard]] inline size_type capacity() const noexcept
        {
            return endArray - beginArray;
        }

        [[nodiscard]] inline size_type front_capacity() const noexcept
        {
            return frontPointer - beginArray;
        }

        [[nodiscard]] inline size_type back_capacity() const noexcept
        {
            return endArray - backPointer - 1;
        }

        [[nodiscard]] inline bool empty() const noexcept
        {
            return dequeSize == 0;
        }

        [[nodiscard]] inline reference front()
        {
            if (dequeSize > 0)
            {
                return *frontPointer;
            } else
            {
                throw std::out_of_range("Deque is empty");
            }
        }

        [[nodiscard]] inline const_reference front() const
        {
            if (dequeSize > 0)
            {
                return *frontPointer;
            } else
            {
                throw std::out_of_range("Deque is empty");
            }
        }

        [[nodiscard]] inline reference quick_front()
        {
            return *frontPointer;
        }

        [[nodiscard]] inline const_reference quick_front() const
        {
            return *frontPointer;
        }

        [[nodiscard]] inline reference back()
        {
            if (dequeSize > 0)
            {
                return *backPointer;
            } else
            {
                throw std::out_of_range("Deque is empty");
            }
        }

        [[nodiscard]] inline const_reference back() const
        {
            if (dequeSize > 0)
            {
                return *backPointer;
            } else
            {
                throw std::out_of_range("Deque is empty");
            }
        }

        [[nodiscard]] inline reference quick_back()
        {
            return *backPointer;
        }

        [[nodiscard]] inline const_reference quick_back() const
        {
            return *backPointer;
        }

        void reserve_front(const size_type newFrontCapacity)
        {
            const size_type frontCapacity = frontPointer - beginArray;
            if (newFrontCapacity > frontCapacity)
            {
                const size_type capacity = endArray - beginArray;
                const size_type newCapacity = capacity + newFrontCapacity - frontCapacity;
                pointer workArray{nullptr};
                pointer workIterator{nullptr};

                try
                {
                    workArray = allocator.allocate(newCapacity);
                } catch (std::bad_alloc)
                {
                    return;
                }

                workIterator = workArray + newFrontCapacity;

                const pointer begin = frontPointer;
                const pointer end = backPointer + 1;

                for(auto iter = begin; iter != end; ++iter)
                {
                    new(workIterator) value_type(std::move(*iter));
                    ++workIterator;
                }

                allocator.deallocate(beginArray, (endArray - beginArray));
                beginArray = workArray;
                endArray = beginArray + newCapacity;
                frontPointer = beginArray + newFrontCapacity;
                backPointer = frontPointer + dequeSize - 1;
            }
        }

        void shrink_to_fit_front()
        {
            const size_type frontCapacity = frontPointer - beginArray;
            if (frontCapacity > 0)
            {
                const size_type capacity = endArray - beginArray;
                const size_type newCapacity = capacity - frontCapacity;

                pointer workArray{nullptr};
                pointer workIterator{nullptr};

                try
                {
                    workArray = allocator.allocate(newCapacity);
                } catch (std::bad_alloc)
                {
                    return;
                }
                workIterator = workArray;

                const pointer begin = frontPointer;
                const pointer end = backPointer + 1;

                for(auto iter = begin; iter != end; ++iter)
                {
                    new(workIterator) value_type(std::move(*iter));
                    ++workIterator;
                }

                allocator.deallocate(beginArray, capacity);
                beginArray = workArray;
                endArray = beginArray + newCapacity;
                frontPointer = beginArray;
                backPointer = frontPointer + dequeSize - 1;
            }
        }

        void reserve_back(const size_type newBackCapacity)
        {
            const size_type backCapacity = endArray - backPointer - 1;
            if (newBackCapacity > backCapacity)
            {
                const size_type capacity = endArray - beginArray;
                const size_type newCapacity = capacity + newBackCapacity - backCapacity;

                pointer workArray{nullptr};
                pointer workIterator{nullptr};

                try
                {
                    workArray = allocator.allocate(newCapacity);
                } catch (std::bad_alloc)
                {
                    return;
                }

                workIterator = workArray + newCapacity - newBackCapacity - 1;

                const pointer begin = backPointer;
                const pointer end = frontPointer - 1;

                for(auto iter = begin; iter != end; --iter)
                {
                    new(workIterator) value_type(std::move(*iter));
                    --workIterator;
                }

                allocator.deallocate(beginArray, capacity);
                beginArray = workArray;
                endArray = beginArray + newCapacity;
                frontPointer = workIterator + 1;
                backPointer = frontPointer + dequeSize - 1;
            }
        }

        void shrink_to_fit_back()
        {
            const size_type backCapacity = endArray - backPointer - 1;
            if (backCapacity > 0)
            {
                const size_type capacity = endArray - beginArray;
                const size_type newCapacity = capacity - backCapacity;
                const size_type frontCapacity = frontPointer - beginArray;

                pointer workArray{nullptr};
                pointer workIterator{nullptr};

                try
                {
                    workArray = allocator.allocate(newCapacity);
                } catch (std::bad_alloc)
                {
                    return;
                }

                workIterator = workArray + frontCapacity;

                const pointer begin = frontPointer;
                const pointer end = backPointer + 1;

                for(auto iter = begin; iter != end; ++iter)
                {
                    new(workIterator) value_type(std::move(*iter));
                    ++workIterator;
                }

                allocator.deallocate(beginArray, capacity);
                beginArray = workArray;
                endArray = beginArray + newCapacity;
                frontPointer = beginArray + frontCapacity;
                backPointer = frontPointer + dequeSize - 1;
            }
        }

        void push_front(const_reference value)
        {
            if (dequeSize == 0 && beginArray == nullptr)
            {
                pointer workArray{nullptr};

                try
                {
                    workArray = allocator.allocate(1);
                    new(workArray) value_type(value);
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workArray).~value_type();
                    allocator.deallocate(workArray, 1);
                    return;
                }
                dequeSize = 1;
                beginArray = workArray;
                endArray = beginArray + 1;
                frontPointer = beginArray;
                backPointer = frontPointer;
                return;
            }

            const size_type frontCapacity = frontPointer - beginArray;
            const size_type capacity = endArray - beginArray;
            if (frontCapacity == 0)
            {
                if (capacity == 1)
                {
                    reserve_front((2));
                } else if (capacity == 2)
                {
                    reserve_front(4);
                } else
                {
                    reserve_front(capacity * 1.5);
                }
            }

            pointer workIterator = frontPointer - 1;
            try
            {
                new(workIterator) value_type(value);
            } catch (...)
            {
                return;
            }
            ++dequeSize;
            --frontPointer;
        }

        void push_front(value_type&& value)
        {
            if (dequeSize == 0 && beginArray == nullptr)
            {
                pointer workArray{nullptr};

                try
                {
                    workArray = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    return;
                }
                new(workArray) value_type(std::move(value));

                dequeSize = 1;
                beginArray = workArray;
                endArray = beginArray + 1;
                frontPointer = beginArray;
                backPointer = frontPointer;
                return;
            }

            const size_type frontCapacity = frontPointer - beginArray;
            const size_type capacity = endArray - beginArray;
            if (frontCapacity == 0)
            {
                if ( capacity == 1)
                {
                    reserve_front((2));
                } else if (capacity == 2)
                {
                    reserve_front(4);
                } else
                {
                    reserve_front(frontCapacity * 1.5);
                }
            }

            pointer workIterator = frontPointer - 1;
            new(workIterator) value_type(std::move(value));
            ++dequeSize;
            --frontPointer;
        }

        template<typename... Args>
        void emplace_front(Args... args)
        {
            if (dequeSize == 0 && beginArray == nullptr)
            {
                pointer workArray{nullptr};

                try
                {
                    workArray = allocator.allocate(1);
                    new(workArray) value_type(std::forward<Args>(args)...);
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workArray).~value_type();
                    allocator.deallocate(workArray, 1);
                    return;
                }
                dequeSize = 1;
                beginArray = workArray;
                endArray = beginArray + 1;
                frontPointer = beginArray;
                backPointer = frontPointer;
                return;
            }

            const size_type frontCapacity = frontPointer - beginArray;
            const size_type capacity = endArray - beginArray;

            if (frontCapacity == 0)
            {
                if (capacity == 1)
                {
                    reserve_front(2);
                } else if (capacity == 2)
                {
                    reserve_front(4);
                } else
                {
                    reserve_front(capacity * 1.5);
                }
            }

            pointer workIterator = frontPointer - 1;
            try
            {
                new(workIterator) value_type(std::forward<Args>(args)...);
            } catch (...)
            {
                return;
            }
            ++dequeSize;
            --frontPointer;
        }

        void pop_front()
        {
            if (dequeSize > 0)
            {
                (*frontPointer).~value_type();
                --dequeSize;
                ++frontPointer;
            }
        }

        void push_back(const_reference value)
        {
            if (dequeSize == 0 && beginArray == nullptr)
            {
                pointer workArray{nullptr};

                try
                {
                    workArray = allocator.allocate(1);
                    new(workArray) value_type(value);
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workArray).~value_type();
                    allocator.deallocate(workArray, 1);
                    return;
                }
                dequeSize = 1;
                beginArray = workArray;
                endArray = beginArray + 1;
                frontPointer = beginArray;
                backPointer = frontPointer;
                return;
            }

            const size_type backCapacity = endArray - backPointer - 1;
            const size_type capacity = endArray - beginArray;
            if (backCapacity == 0)
            {
                if (capacity == 1)
                {
                    reserve_back((2));
                } else if (capacity == 2)
                {
                    reserve_back(4);
                } else
                {
                    reserve_back(capacity * 0.5);
                }
            }

            pointer workIterator = backPointer + 1;
            try
            {
                new(workIterator) value_type(value);
            } catch (...)
            {
                return;
            }
            ++dequeSize;
            ++backPointer;
        }

        void push_back(value_type&& value)
        {
            if (dequeSize == 0 && beginArray == nullptr)
            {
                pointer workArray{nullptr};

                try
                {
                    workArray = allocator.allocate(1);

                } catch (std::bad_alloc)
                {
                    return;
                }

                new(workArray) value_type(std::move(value));

                dequeSize = 1;
                beginArray = workArray;
                endArray = beginArray + 1;
                frontPointer = beginArray;
                backPointer = frontPointer;
                return;
            }

            const size_type backCapacity = endArray - backPointer - 1;
            const size_type capacity = endArray - beginArray;
            if (backCapacity == 0)
            {
                if (capacity == 1)
                {
                    reserve_back((2));
                } else if (capacity == 2)
                {
                    reserve_back(4);
                } else
                {
                    reserve_back(capacity * 0.5);
                }
            }

            pointer workIterator = backPointer + 1;
            new(workIterator) value_type(std::move(value));
            ++dequeSize;
            ++backPointer;
        }

        template<typename... Args>
        void emplace_back(Args... args)
        {
            if (dequeSize == 0 && beginArray == nullptr)
            {
                pointer workArray{nullptr};

                try
                {
                    workArray = allocator.allocate(1);
                    new(workArray) value_type(std::forward<Args>(args)...);
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workArray).~value_type();
                    allocator.deallocate(workArray, 1);
                    return;
                }
                dequeSize = 1;
                beginArray = workArray;
                endArray = beginArray + 1;
                frontPointer = beginArray;
                backPointer = frontPointer;
                return;
            }

            const size_type backCapacity = endArray - backPointer - 1;
            const size_type capacity = endArray - beginArray;
            if (backCapacity == 0)
            {
                if (capacity == 1)
                {
                    reserve_back((2));
                } else if (capacity == 2)
                {
                    reserve_back(4);
                } else
                {
                    reserve_back(capacity * 0.5);
                }
            }

            pointer workIterator = backPointer + 1;
            try
            {
                new(workIterator) value_type(std::forward<Args>(args)...);
            } catch (...)
            {
                return;
            }
            ++dequeSize;
            ++backPointer;
        }

        void pop_back()
        {
            if (dequeSize > 0)
            {
                (*backPointer).~value_type();
                --dequeSize;
                --backPointer;
            }
        }

        void swap(DequeVector<TYPE, ALLOCATOR>& other)
        {
            const auto tempSize = dequeSize;
            const auto tempBegin = beginArray;
            const auto tempEnd = endArray;
            const auto tempFront = frontPointer;
            const auto tempBack = backPointer;

            dequeSize = other.dequeSize;
            beginArray = other.beginArray;
            endArray = other.endArray;
            frontPointer = other.frontPointer;
            backPointer = other.backPointer;

            other.dequeSize = tempSize;
            other.beginArray = tempBegin;
            other.endArray = tempEnd;
            other.frontPointer = tempFront;
            other.backPointer = tempBack;
        }

        [[nodiscard]] bool operator == (const DequeVector<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal = true;

            if (dequeSize == other.dequeSize)
            {
                for(size_type idx = 0; idx < dequeSize; ++idx)
                {
                    if (frontPointer[idx] != other.frontPointer[idx])
                    {
                        equal = false;
                        break;
                    }
                }

            } else
            {
                equal = false;
            }

            return equal;
        }

        [[nodiscard]] bool operator != (const DequeVector<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal = false;

            if (dequeSize == other.dequeSize)
            {
                for(size_type idx = 0; idx < dequeSize; ++idx)
                {
                    if (frontPointer[idx] != other.frontPointer[idx])
                    {
                        equal = true;
                        break;
                    }
                }

            } else
            {
                equal = true;
            }

            return equal;
        }

        [[nodiscard]] bool operator >= (const DequeVector<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (dequeSize == other.dequeSize)
            {
                equal = true;
                pointer left = frontPointer;
                pointer right = other.frontPointer;
                for(std::uint64_t idx = 0; idx < dequeSize; ++idx)
                {
                    if (left[idx] != right[idx])
                    {
                        if (left[idx] > right[idx])
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                }

            } else if (dequeSize > other.dequeSize)
            {
                equal = true;
                pointer left = frontPointer;
                pointer right = other.frontPointer;
                for(std::uint64_t idx = 0; idx < other.dequeSize; ++idx)
                {
                    if (left[idx] != right[idx])
                    {
                        if (left[idx] > right[idx])
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                }

            } else
            {
                equal = false;
                pointer left = frontPointer;
                pointer right = other.frontPointer;
                for(std::uint64_t idx = 0; idx < dequeSize; ++idx)
                {
                    if (left[idx] != right[idx])
                    {
                        if (left[idx] > right[idx])
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                }
            }

            return equal;
        }

        [[nodiscard]] bool operator > (const DequeVector<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (dequeSize == other.dequeSize)
            {
                equal = false;
                pointer left = frontPointer;
                pointer right = other.frontPointer;
                for(std::uint64_t idx = 0; idx < dequeSize; ++idx)
                {
                    if (left[idx] != right[idx])
                    {
                        if (left[idx] > right[idx])
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                }
            } else if (dequeSize > other.dequeSize)
            {
                equal = true;
                pointer left = frontPointer;
                pointer right = other.frontPointer;
                for(std::uint64_t idx = 0; idx < other.dequeSize; ++idx)
                {
                    if (left[idx] != right[idx])
                    {
                        if (left[idx] > right[idx])
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                }
            } else
            {
                equal = false;
                pointer left = frontPointer;
                pointer right = other.frontPointer;
                for(std::uint64_t idx = 0; idx < dequeSize; ++idx)
                {
                    if (left[idx] != right[idx])
                    {
                        if (left[idx] > right[idx])
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                }
            }

            return equal;
        }

        [[nodiscard]] bool operator <= (const DequeVector<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (dequeSize == other.dequeSize)
            {
                equal = true;
                pointer left = frontPointer;
                pointer right = other.frontPointer;
                for(std::uint64_t idx = 0; idx < dequeSize; ++idx)
                {
                    if (left[idx] != right[idx])
                    {
                        if (left[idx] < right[idx])
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                }
            } else if (dequeSize > other.dequeSize)
            {
                equal = false;
                pointer left = frontPointer;
                pointer right = other.frontPointer;
                for(std::uint64_t idx = 0; idx < other.dequeSize; ++idx)
                {
                    if (left[idx] != right[idx])
                    {
                        if (left[idx] < right[idx])
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                }
            } else
            {
                equal = true;
                pointer left = frontPointer;
                pointer right = other.frontPointer;
                for(std::uint64_t idx = 0; idx < dequeSize; ++idx)
                {
                    if (left[idx] != right[idx])
                    {
                        if (left[idx] < right[idx])
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                }
            }

            return equal;
        }

        [[nodiscard]] bool operator < (const DequeVector<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (dequeSize == other.dequeSize)
            {
                equal = false;
                pointer left = frontPointer;
                pointer right = other.frontPointer;
                for(std::uint64_t idx = 0; idx < dequeSize; ++idx)
                {
                    if (left[idx] != right[idx])
                    {
                        if (left[idx] < right[idx])
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                }
            } else if (dequeSize > other.dequeSize)
            {
                equal = false;
                pointer left = frontPointer;
                pointer right = other.frontPointer;
                for(std::uint64_t idx = 0; idx < other.dequeSize; ++idx)
                {
                    if (left[idx] != right[idx])
                    {
                        if (left[idx] < right[idx])
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                }
            } else
            {
                equal = true;
                pointer left = frontPointer;
                pointer right = other.frontPointer;
                for(std::uint64_t idx = 0; idx < dequeSize; ++idx)
                {
                    if (left[idx] != right[idx])
                    {
                        if (left[idx] < right[idx])
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                }
            }

            return equal;
        }

    private:

        allocator_type allocator;
        size_type dequeSize;
        pointer beginArray;
        pointer endArray;
        pointer frontPointer;
        pointer backPointer;

    };
}

#endif //INC_3DGAME_DEQUE_VECTOR_H
