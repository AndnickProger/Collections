#ifndef INC_3DGAME_STACK_VECTOR_H
#define INC_3DGAME_STACK_VECTOR_H


#include <stdexcept>
#include <cstdint>
#include <cstdlib>

namespace bice
{

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    class StackVector
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

        StackVector(allocator_type allocator_ = ALLOCATOR()) :
                allocator(allocator_),
                stackSize{}, stackCapacity{},
                array{}, topPointer{}
        {}

        explicit StackVector(const size_type size, allocator_type allocator_ = ALLOCATOR()) :
                allocator(allocator_),
                stackSize{}, stackCapacity{},
                array{}, topPointer{}
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
                    allocator.deallocate(workArray);
                    return;
                }
                stackSize = size;
                stackCapacity = size;
                array = workArray;
                topPointer = array + stackSize - 1;
            }
        }

        StackVector(const size_type size, const_reference value, allocator_type allocator_ = ALLOCATOR()) :
                allocator(allocator_),
                stackSize{}, stackCapacity{},
                array{}, topPointer{}
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
                    allocator.deallocate(workArray);
                    return;
                }
                stackSize = size;
                stackCapacity = size;
                array = workArray;
                topPointer = array + stackSize - 1;
            }
        }

        StackVector(std::initializer_list<value_type> list, allocator_type allocator_ = ALLOCATOR()) :
                allocator(allocator_),
                stackSize{}, stackCapacity{},
                array{}, topPointer{}
        {
            if (list.size() > 0)
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
                }
                stackSize = list.size();
                stackCapacity = stackSize;
                array = workArray;
                topPointer = array + stackSize - 1;
            }
        }

        template<typename ITERATOR>
        StackVector(const ITERATOR begin, const ITERATOR end, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        stackSize{}, stackCapacity{},
        array{}, topPointer{}
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
                        new(workIterator) value_type (*iter);
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
                stackSize = distance;
                stackCapacity = distance;
                array = workArray;
                topPointer = array + stackSize - 1;
            }
        }

        StackVector(const StackVector<TYPE, ALLOCATOR>& other)
        {
            stackSize = 0;
            stackCapacity = 0;
            array = nullptr;
            topPointer = nullptr;
            if (other.stackSize > 0)
            {
                pointer workArray{nullptr};
                pointer workIterator{nullptr};

                try
                {
                    workArray = allocator.allocate(other.stackSize);
                    workIterator = workArray;

                    for(size_type idx = 0; idx < other.stackSize; ++idx)
                    {
                        new(workIterator) value_type(other.array[idx]);
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
                    allocator.deallocate(workArray, other.stackSize);
                    return;
                }
                stackSize = other.stackSize;
                stackCapacity = other.stackSize;
                array = workArray;
                topPointer = array + stackSize - 1;
            }
        }

        StackVector<TYPE, ALLOCATOR>&operator = (const StackVector<TYPE, ALLOCATOR>& other)
        {
            if (stackSize == other.stackSize && stackSize > 0)
            {
                for(size_type idx = 0; idx < stackSize; ++idx)
                {
                    array[idx] = other.array[idx];
                }
            } else if (stackSize != other.stackSize && other.stackSize <= stackCapacity && stackSize > 0)
            {
                for(size_type idx = 0; idx < stackSize; ++idx)
                {
                    array[idx].~value_type();
                }

                pointer workPointer = array;
                for(size_type idx; idx < other.stackSize; ++idx)
                {
                    new(workPointer) value_type(other.array[idx]);
                    ++workPointer;
                }
            } else
            {
                for(size_type idx = 0; idx < stackSize; ++idx)
                {
                    array[idx].~value_type();
                }
                if (array != nullptr)
                {
                    allocator.deallocate(array, stackCapacity);
                }

                stackSize = 0;
                stackCapacity = 0;
                array = nullptr;
                topPointer = nullptr;

                if (other.stackSize > 0)
                {
                    pointer workArray{nullptr};
                    pointer workIterator{nullptr};

                    try
                    {
                        workArray = allocator.allocate(other.stackSize);
                        workIterator = workArray;

                        for(size_type idx = 0; idx < other.stackSize; ++idx)
                        {
                            new(workIterator) value_type(other.array[idx]);
                            ++workIterator;
                        }
                    } catch (std::bad_alloc)
                    {
                        return *this;
                    } catch (...)
                    {
                        for(auto iter = workArray; iter != workIterator; ++iter)
                        {
                            (*iter).~value_type();
                        }
                        allocator.deallocate(workArray, other.stackSize);
                        return *this;
                    }
                    stackSize = other.stackSize;
                    stackCapacity = other.stackSize;
                    array = workArray;
                    topPointer = array + stackSize - 1;
                }
            }


            return *this;
        }

        StackVector(StackVector<TYPE, ALLOCATOR>&& other) noexcept :
        stackSize(std::move(other.stackSize)),
        stackCapacity(std::move(other.stackCapacity)),
        array(std::move(other.array)),
        topPointer(std::move(other.topPointer))
        {
            other.stackSize = 0;
            other.stackCapacity = 0;
            other.array = nullptr;
            other.topPointer = nullptr;
        }

        StackVector<TYPE, ALLOCATOR>&operator = (StackVector<TYPE, ALLOCATOR>&& other) noexcept
        {
            if(stackSize > 0)
            {
                for(size_type idx = 0; idx < stackSize; ++idx)
                {
                    array[idx].~value_type();
                }
            }

            if (array != nullptr)
            {
                allocator.deallocate(array, stackCapacity);
            }

            stackSize = std::move(other.stackSize);
            stackCapacity = std::move(other.stackCapacity);
            array = std::move(other.array);
            topPointer = std::move(other.topPointer);

            other.stackSize = 0;
            other.stackCapacity = 0;
            other.array = nullptr;
            other.topPointer = nullptr;

            return *this;
        }

        virtual ~StackVector()
        {
            if(stackSize > 0)
            {
                for(size_type idx = 0; idx < stackSize; ++idx)
                {
                    array[idx].~value_type();
                }
            }
            if (array != nullptr)
            {
                allocator.deallocate(array, stackCapacity);
            }
        }

        [[nodiscard]] inline size_type size() const noexcept
        {
            return stackSize;
        }

        [[nodiscard]] inline size_type capacity() const noexcept
        {
            return stackCapacity;
        }

        [[nodiscard]] inline bool empty() const noexcept
        {
            return stackSize == 0;
        }

        [[nodiscard]] inline reference top()
        {
            if (stackSize > 0)
            {
                return *topPointer;
            } else
            {
                throw std::out_of_range("Stack is empty");
            }
        }

        [[nodiscard]] inline const_reference top() const
        {
            if (stackSize > 0)
            {
                return *topPointer;
            } else
            {
                throw std::out_of_range("Stack is empty");
            }
        }

        [[nodiscard]] inline reference quick_top()
        {
            return *topPointer;
        }

        [[nodiscard]] inline const_reference quick_top() const
        {
            return *topPointer;
        }

        void reserve(const size_type newCapacity)
        {
            if (newCapacity > stackCapacity)
            {
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

                for(size_type idx = 0; idx < stackSize; ++idx)
                {
                    new(workIterator) value_type(std::move(array[idx]));
                    ++workIterator;
                }

                allocator.deallocate(array, stackCapacity);
                array = workArray;
                stackCapacity = newCapacity;
                topPointer = array + stackSize - 1;
            }
        }

        void shrink_to_fit()
        {
            if (stackSize <= stackCapacity)
            {
                pointer workArray{nullptr};
                pointer workIterator;
                try
                {
                    workArray = allocator.allocate(stackSize);

                } catch (std::bad_alloc)
                {
                    return;
                }

                workIterator = workArray;

                for(size_type idx = 0; idx < stackSize; ++idx)
                {
                    new(workIterator) value_type(std::move(array[idx]));
                    ++workIterator;
                }

                allocator.deallocate(array, stackCapacity);
                array = workArray;
                stackCapacity = stackSize;
                topPointer = array + stackSize - 1;
            }
        }

        void push(const_reference value)
        {
            if (stackSize == 0 && array == nullptr)
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
                    allocator.deallocate(workArray, 1);
                    return;
                }

                stackSize = 1;
                stackCapacity = 1;
                array = workArray;
                topPointer = array;
                return;
            }

            if (stackSize == stackCapacity)
            {
                if (stackCapacity == 1)
                {
                    reserve(2);
                } else if (stackCapacity == 2)
                {
                    reserve(4);
                } else
                {
                    reserve(stackCapacity * 1.5);
                }
            }

            pointer workIterator = topPointer + 1;
            try
            {
                new(workIterator) value_type(value);
            } catch (...)
            {
                return;
            }

            ++stackSize;
            ++topPointer;
        }

        void push(value_type&& value)
        {
            if (stackSize == 0 && array == nullptr)
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
                    allocator.deallocate(workArray, 1);
                    return;
                }

                stackSize = 1;
                stackCapacity = 1;
                array = workArray;
                topPointer = array;
                return;
            }

            if (stackSize == stackCapacity)
            {
                if (stackCapacity == 1)
                {
                    reserve(2);
                } else if (stackCapacity == 2)
                {
                    reserve(4);
                } else
                {
                    reserve(stackCapacity * 1.5);
                }
            }

            pointer workIterator = topPointer + 1;
            new(workIterator) value_type(std::move(value));

            ++stackSize;
            ++topPointer;
        }

        template<typename... Args>
        void emplace(Args... args)
        {
            if (stackSize == 0 && array == nullptr)
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
                    allocator.deallocate(workArray, 1);
                    return;
                }

                stackSize = 1;
                stackCapacity = 1;
                array = workArray;
                topPointer = array;
                return;
            }

            if (stackSize == stackCapacity)
            {
                if (stackCapacity == 1)
                {
                    reserve(2);
                } else if (stackCapacity == 2)
                {
                    reserve(4);
                } else
                {
                    reserve(stackCapacity * 1.5);
                }
            }

            pointer workIterator = topPointer + 1;
            try
            {
                new(workIterator) value_type(std::forward<Args>(args)...);
            } catch (...)
            {
                return;
            }

            ++stackSize;
            ++topPointer;
        }

        void pop()
        {
            if (stackSize > 0)
            {
                (*topPointer).~value_type();
                --stackSize;
                --topPointer;
            }
        }

        void quick_pop()
        {
            (*topPointer).~value_type();
            --stackSize;
            --topPointer;
        }

        void swap(StackVector<TYPE, ALLOCATOR>& other)
        {
            const auto tempSize = stackSize;
            const auto tempCapacity = stackCapacity;
            const auto tempArray = array;
            const auto tempTop = topPointer;

            stackSize = other.stackSize;
            stackCapacity = other.stackCapacity;
            array = other.array;
            topPointer = other.topPointer;

            other.stackSize = tempSize;
            other.stackCapacity = tempCapacity;
            other.array = tempArray;
            other.topPointer = topPointer;
        }

        [[nodiscard]] bool operator == (const StackVector<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal = true;

            if (stackSize == other.stackSize)
            {
                for(size_type idx = 0; idx < stackSize; ++idx)
                {
                    if (array[idx] != other.array[idx])
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

        [[nodiscard]] bool operator != (const StackVector<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal = false;

            if (stackSize == other.stackSize)
            {
                for(size_type idx = 0; idx < stackSize; ++idx)
                {
                    if (array[idx] != other.array[idx])
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

        [[nodiscard]] bool operator >= (const StackVector<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (stackSize == other.stackSize)
            {
                equal = true;
                pointer left = array;
                pointer right = other.array;
                for(std::uint64_t idx = 0; idx < stackSize; ++idx)
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

            } else if (stackSize > other.stackSize)
            {
                equal = true;
                pointer left = array;
                pointer right = other.array;
                for(std::uint64_t idx = 0; idx < other.stackSize; ++idx)
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
                pointer left = array;
                pointer right = other.array;
                for(std::uint64_t idx = 0; idx < stackSize; ++idx)
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

        [[nodiscard]] bool operator > (const StackVector<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (stackSize == other.stackSize)
            {
                equal = false;
                pointer left = array;
                pointer right = other.array;
                for(std::uint64_t idx = 0; idx < stackSize; ++idx)
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
            } else if (stackSize > other.stackSize)
            {
                equal = true;
                pointer left = array;
                pointer right = other.array;
                for(std::uint64_t idx = 0; idx < other.stackSize; ++idx)
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
                pointer left = array;
                pointer right = other.array;
                for(std::uint64_t idx = 0; idx < stackSize; ++idx)
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

        [[nodiscard]] bool operator <= (const StackVector<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (stackSize == other.stackSize)
            {
                equal = true;
                pointer left = array;
                pointer right = other.array;
                for(std::uint64_t idx = 0; idx < stackSize; ++idx)
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
            } else if (stackSize > other.stackSize)
            {
                equal = false;
                pointer left = array;
                pointer right = other.array;
                for(std::uint64_t idx = 0; idx < other.stackSize; ++idx)
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
                pointer left = array;
                pointer right = other.array;
                for(std::uint64_t idx = 0; idx < stackSize; ++idx)
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

        [[nodiscard]] bool operator < (const StackVector<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (stackSize == other.stackSize)
            {
                equal = false;
                pointer left = array;
                pointer right = other.array;
                for(std::uint64_t idx = 0; idx < stackSize; ++idx)
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
            } else if (stackSize > other.stackSize)
            {
                equal = false;
                pointer left = array;
                pointer right = other.array;
                for(std::uint64_t idx = 0; idx < other.stackSize; ++idx)
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
                pointer left = array;
                pointer right = other.array;
                for(std::uint64_t idx = 0; idx < stackSize; ++idx)
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
        size_type stackSize;
        size_type stackCapacity;
        pointer array;
        pointer topPointer;
    };
}

#endif //INC_3DGAME_STACK_VECTOR_H
