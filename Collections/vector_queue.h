#ifndef THREADPOOL_VECTOR_QUEUE_H
#define THREADPOOL_VECTOR_QUEUE_H

#include <stdexcept>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <iostream>
#include <memory>

namespace th_pool
{
    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    class VectorQueue {
    public:

        using value_type = TYPE;
        using size_type = std::uint64_t;
        using difference_type = std::int64_t;
        using allocator_type = ALLOCATOR;
        using reference = TYPE &;
        using const_reference = const TYPE &;
        using pointer = TYPE *;
        using const_pointer = const TYPE *;

        VectorQueue(allocator_type allocator_ = allocator_type()) :
                allocator(allocator_),
                queueSize{},
                beginArray{}, endArray{},
                frontPtr{}, backPtr{} {}

        explicit VectorQueue(const_reference value, allocator_type allocator_ = allocator_type()) :
                allocator(allocator_),
                queueSize{},
                beginArray{}, endArray{},
                frontPtr{}, backPtr{} {
            pointer workArray{nullptr};
            try {
                workArray = allocator.allocate(1);
                new(workArray) value_type(value);
            } catch (std::bad_alloc) {
                workArray = nullptr;
                return;
            } catch (...) {
                (*workArray).~value_type();
                allocator.deallocate(workArray, 1);
                workArray = nullptr;
                return;
            }
            queueSize = 1;
            beginArray = workArray;
            endArray = beginArray + 1;
            frontPtr = beginArray;
            backPtr = frontPtr;
        }

        VectorQueue(std::initializer_list<value_type> list, allocator_type allocator_ = allocator_type()) :
                allocator(allocator_),
                queueSize{},
                beginArray{}, endArray{},
                frontPtr{}, backPtr{} {
            pointer workArray{nullptr};
            pointer workIterator{nullptr};

            try {
                workArray = allocator.allocate(list.size());
                workIterator = workArray;
                for (auto iter = list.begin(); iter != list.end(); ++iter) {
                    new(workIterator) value_type(*iter);
                    ++workIterator;
                }

            } catch (std::bad_alloc) {
                workArray = nullptr;
                return;
            } catch (...) {
                for (auto iter = workArray; iter != workIterator; ++iter) {
                    (*iter).~value_type();
                }
                allocator.deallocate(workArray, list.size());
                workArray = nullptr;
                return;
            }
            queueSize = list.size();
            beginArray = workArray;
            endArray = beginArray + queueSize;
            frontPtr = beginArray;
            backPtr = frontPtr + queueSize - 1;
        }

        template<typename ITERATOR>
        VectorQueue(ITERATOR begin, ITERATOR end, allocator_type allocator_ = allocator_type()) :
                allocator(allocator_),
                queueSize{},
                beginArray{}, endArray{},
                frontPtr{}, backPtr{} {
            difference_type distance{0};
            for (auto iter = begin; iter != end; ++iter) {
                ++distance;
            }

            if (distance > 0) {
                pointer workArray{nullptr};
                pointer workIterator{nullptr};
                try {
                    workArray = allocator.allocate(distance);
                    workIterator = workArray;
                    for (auto iter = begin; iter != end; ++iter) {
                        new(workIterator) value_type(*iter);
                        ++workIterator;
                    }
                } catch (std::bad_alloc) {
                    workArray = nullptr;
                    return;
                } catch (...) {
                    for (auto iter = workArray; iter != workIterator; ++iter) {
                        (*iter).~value_type();
                    }
                    allocator.deallocate(workArray, distance);
                    workArray = nullptr;
                    workIterator = nullptr;
                    return;
                }

                queueSize = distance;
                beginArray = workArray;
                endArray = beginArray + queueSize;
                frontPtr = beginArray;
                backPtr = endArray - 1;
            } else {
                throw std::out_of_range("The distance between iterators must be positive");
            }
        }

        VectorQueue(const VectorQueue<TYPE, ALLOCATOR>& other)
        {
            queueSize = other.queueSize;
            if (queueSize > 0)
            {
                pointer workArray{nullptr};
                pointer workIterator{nullptr};

                try
                {
                    workArray = allocator.allocate(queueSize);
                    workIterator = workArray;

                    pointer begin = other.frontPtr;
                    pointer end = other.backPtr + 1;

                    for(auto iter = begin; iter != end; ++iter)
                    {
                        new(workIterator) value_type(*iter);
                        ++workIterator;
                    }

                } catch (std::bad_alloc)
                {
                    workArray = nullptr;
                    queueSize = 0;
                    beginArray = nullptr;
                    endArray = nullptr;
                    frontPtr = nullptr;
                    backPtr = nullptr;
                    return;
                } catch (...)
                {
                    for(auto iter = workArray; iter != workIterator; ++iter)
                    {
                        (*iter).~value_type();
                    }
                    allocator.deallocate(workArray, queueSize);
                    workArray = nullptr;
                    queueSize = 0;
                    beginArray = nullptr;
                    endArray = nullptr;
                    frontPtr = nullptr;
                    backPtr = nullptr;
                }

                beginArray = workArray;
                endArray = beginArray + queueSize;
                frontPtr = beginArray;
                backPtr = frontPtr + queueSize - 1;
            } else
            {
                beginArray = nullptr;
                endArray = nullptr;
                frontPtr = nullptr;
                backPtr = nullptr;
            }
        }

        VectorQueue<TYPE, ALLOCATOR>&operator = (const VectorQueue<TYPE, ALLOCATOR>& other)
        {
            if (queueSize == other.queueSize)
            {
                pointer workIterator = frontPtr;
                pointer begin = other.frontPtr;
                pointer end = other.backPtr + 1;

                try
                {
                    for(auto iter = begin; iter != end; ++iter)
                    {
                        *workIterator = *iter;
                        ++workIterator;
                    }
                } catch (...)
                {
                    for(auto iter = frontPtr; iter != workIterator; ++iter)
                    {
                        (*iter).~value_type();
                    }
                    queueSize = 0;
                    frontPtr = beginArray;
                    backPtr = frontPtr;
                    return *this;
                }
            } else
            {
                if (beginArray != nullptr)
                {
                    for(auto iter = frontPtr; iter != (backPtr + 1); ++iter)
                    {
                        (*iter).~value_type();
                    }
                    allocator.deallocate(beginArray, (endArray - beginArray));
                }

                queueSize = other.queueSize;
                if (queueSize > 0)
                {
                    pointer workArray{nullptr};
                    pointer workIterator{nullptr};

                    try
                    {
                        workArray = allocator.allocate(queueSize);
                        workIterator = workArray;

                        pointer begin = other.frontPtr;
                        pointer end = other.backPtr + 1;

                        for(auto iter = begin; iter != end; ++iter)
                        {
                            new(workIterator) value_type(*iter);
                            //*workIterator = *iter;
                            ++workIterator;
                        }

                    } catch (std::bad_alloc)
                    {
                        workArray = nullptr;
                        queueSize = 0;
                        beginArray = nullptr;
                        endArray = nullptr;
                        frontPtr = nullptr;
                        backPtr = nullptr;
                        return *this;
                    } catch (...)
                    {
                        for(auto iter = workArray; iter != workIterator; ++iter)
                        {
                            (*iter).~value_type();
                        }
                        allocator.deallocate(workArray, queueSize);
                        workArray = nullptr;
                        queueSize = 0;
                        beginArray = nullptr;
                        endArray = nullptr;
                        frontPtr = nullptr;
                        backPtr = nullptr;
                        return *this;
                    }
                    beginArray = workArray;
                    endArray = beginArray + queueSize;
                    frontPtr = beginArray;
                    backPtr = frontPtr + queueSize - 1;
                } else
                {
                    beginArray = nullptr;
                    endArray = nullptr;
                    frontPtr = nullptr;
                    backPtr = nullptr;
                }
            }

            return *this;
        }

        VectorQueue(VectorQueue<TYPE, ALLOCATOR> &&other) noexcept:
                queueSize(std::move(other.queueSize)),
                beginArray(std::move(other.beginArray)),
                endArray(std::move(other.endArray)),
                frontPtr(std::move(other.frontPtr)),
                backPtr(std::move(other.backPtr)) {
            other.queueSize = 0;
            other.beginArray = nullptr;
            other.endArray = nullptr;
            other.frontPtr = nullptr;
            other.backPtr = nullptr;
        }

        VectorQueue<TYPE, ALLOCATOR> &operator=(VectorQueue<TYPE, ALLOCATOR> &&other) noexcept
        {
            if (beginArray != nullptr)
            {
                for(auto iter = frontPtr; iter != (backPtr + 1); ++iter)
                {
                    (*iter).~value_type();
                }
                allocator.deallocate(beginArray, (endArray - beginArray));
            }

            queueSize = std::move(other.queueSize);
            beginArray = std::move(other.beginArray);
            endArray = std::move(other.endArray);
            frontPtr = std::move(other.frontPtr);
            backPtr = std::move(other.backPtr);

            other.queueSize = 0;
            other.beginArray = nullptr;
            other.endArray = nullptr;
            other.frontPtr = nullptr;
            other.backPtr = nullptr;

            return *this;
        }

        virtual ~VectorQueue()
        {
            if (beginArray != nullptr)
            {
                for(auto iter = frontPtr; iter != (backPtr + 1); ++iter)
                {
                    (*iter).~value_type();
                }

                allocator.deallocate(beginArray, (endArray - beginArray));

                queueSize = 0;
                beginArray = nullptr;
                endArray = nullptr;
                frontPtr = nullptr;
                backPtr = nullptr;
            }
        }

        [[nodiscard]] inline bool empty() const noexcept
        {
            return queueSize == 0;
        }

        [[nodiscard]] inline size_type size() const noexcept
        {
            return queueSize;
        }

        [[nodiscard]] inline reference front()
        {
            if (queueSize > 0)
            {
                return *frontPtr;
            } else
            {
                throw std::out_of_range("Queue is empty");
            }
        }

        [[nodiscard]] inline const_reference front() const
        {
            if (queueSize > 0)
            {
                return *frontPtr;
            } else
            {
                throw std::out_of_range("Queue is empty");
            }
        }

        [[nodiscard]] inline reference back()
        {
            if (queueSize > 0)
            {
                return *backPtr;
            } else
            {
                throw std::out_of_range("Queue is empty");
            }
        }

        [[nodiscard]] inline const_reference back() const
        {
            if (queueSize > 0)
            {
                return *backPtr;
            } else
            {
                throw std::out_of_range("Queue is empty");
            }
        }

        void push(const_reference value)
        {
            if (queueSize == 0 && beginArray == nullptr)
            {
                pointer workArray{nullptr};
                try
                {
                    workArray = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    workArray = nullptr;
                    return;
                }
                queueSize = 0;
                beginArray = workArray;
                endArray = beginArray + 1;
                frontPtr = beginArray;
                backPtr = frontPtr;
            }

            if (backPtr + 1 == endArray && queueSize > 0)
            {
                const difference_type buffer = frontPtr - beginArray;
                if (buffer > 0)
                {
                    for(size_type idx = 0; idx < queueSize; ++idx)
                    {
                        beginArray[idx] = std::move(beginArray[idx + buffer]);
                    }
                    frontPtr = beginArray;
                    backPtr = frontPtr + queueSize - 1;
                } else
                {
                    const difference_type capacity = endArray - beginArray;
                    const difference_type newCapacity = capacity * 2;

                    pointer workArray{nullptr};
                    pointer workIterator{nullptr};
                    try
                    {
                        workArray = allocator.allocate(newCapacity);
                    } catch (std::bad_alloc)
                    {
                        workArray = nullptr;
                        return;
                    }
                    workIterator = workArray;
                    for(auto iter = frontPtr; iter != (backPtr + 1); ++iter)
                    {
                        new(workIterator) value_type(std::move(*iter));
                        (*iter).~value_type();
                        ++workIterator;
                    }

                    allocator.deallocate(beginArray, capacity);
                    beginArray = workArray;
                    endArray = beginArray + newCapacity;
                    frontPtr = beginArray;
                    backPtr = frontPtr + queueSize - 1;

                }
            }

            if (queueSize == 0)
            {
                try
                {
                    new(frontPtr) value_type(value);
                } catch (...)
                {
                    queueSize = 0;
                    backPtr = frontPtr;
                    return;
                }

                backPtr = frontPtr;
                queueSize = 1;
            } else
            {
                try
                {
                    ++backPtr;
                    new(backPtr) value_type(value);
                } catch (...)
                {
                    --backPtr;
                    return;
                }
                ++queueSize;
            }
        }

        void push(value_type&& value)
        {
            if (queueSize == 0 && beginArray == nullptr)
            {
                pointer workArray{nullptr};
                try
                {
                    workArray = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    workArray = nullptr;
                    return;
                }
                queueSize = 0;
                beginArray = workArray;
                endArray = beginArray + 1;
                frontPtr = beginArray;
                backPtr = frontPtr;
            }

            if (backPtr + 1 == endArray && queueSize > 0)
            {
                const difference_type buffer = frontPtr - beginArray;
                if (buffer > 0)
                {
                    for(size_type idx = 0; idx < queueSize; ++idx)
                    {
                        beginArray[idx] = std::move(beginArray[idx + buffer]);
                    }
                    frontPtr = beginArray;
                    backPtr = frontPtr + queueSize - 1;
                } else
                {
                    const difference_type capacity = endArray - beginArray;
                    const difference_type newCapacity = capacity * 2;

                    pointer workArray{nullptr};
                    pointer workIterator{nullptr};
                    try
                    {
                        workArray = allocator.allocate(newCapacity);
                    } catch (std::bad_alloc)
                    {
                        workArray = nullptr;
                        return;
                    }
                    workIterator = workArray;
                    for(auto iter = frontPtr; iter != (backPtr + 1); ++iter)
                    {
                        new(workIterator) value_type(std::move(*iter));
                        (*iter).~value_type();
                        ++workIterator;
                    }

                    allocator.deallocate(beginArray, capacity);
                    beginArray = workArray;
                    endArray = beginArray + newCapacity;
                    frontPtr = beginArray;
                    backPtr = frontPtr + queueSize - 1;

                }
            }

            if (queueSize == 0)
            {
                try
                {
                    new(frontPtr) value_type(std::move(value));
                } catch (...)
                {
                    queueSize = 0;
                    backPtr = frontPtr;
                    return;
                }

                backPtr = frontPtr;
                queueSize = 1;
            } else
            {
                try
                {
                    ++backPtr;
                    new(backPtr) value_type(std::move(value));
                } catch (...)
                {
                    --backPtr;
                    return;
                }
                ++queueSize;
            }
        }

        void pop()
        {
            if (queueSize > 0)
            {
                (*frontPtr).~value_type();
                ++frontPtr;
                --queueSize;
            }
        }

        void swap(VectorQueue<TYPE, ALLOCATOR>& other)
        {
            auto tempSize = queueSize;
            auto tempBegin = beginArray;
            auto tempEnd = endArray;
            auto tempFront = frontPtr;
            auto tempBack = backPtr;

            queueSize = other.queueSize;
            beginArray = other.beginArray;
            endArray = other.endArray;
            frontPtr = other.frontPtr;
            backPtr = other.backPtr;

            other.queueSize = tempSize;
            other.beginArray = tempBegin;
            other.endArray = tempEnd;
            other.frontPtr = tempFront;
            other.backPtr = tempBack;
        }

        template<typename... Args>
        void emplace(Args... args)
        {
            if (queueSize == 0 && beginArray == nullptr)
            {
                pointer workArray{nullptr};
                try
                {
                    workArray = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    workArray = nullptr;
                    return;
                }
                queueSize = 0;
                beginArray = workArray;
                endArray = beginArray + 1;
                frontPtr = beginArray;
                backPtr = frontPtr;
            }

            if (backPtr + 1 == endArray && queueSize > 0)
            {
                const difference_type buffer = frontPtr - beginArray;
                if (buffer > 0)
                {
                    for(size_type idx = 0; idx < queueSize; ++idx)
                    {
                        beginArray[idx] = std::move(beginArray[idx + buffer]);
                    }
                    frontPtr = beginArray;
                    backPtr = frontPtr + queueSize - 1;
                } else
                {
                    const difference_type capacity = endArray - beginArray;
                    const difference_type newCapacity = capacity * 2;

                    pointer workArray{nullptr};
                    pointer workIterator{nullptr};
                    try
                    {
                        workArray = allocator.allocate(newCapacity);
                    } catch (std::bad_alloc)
                    {
                        workArray = nullptr;
                        return;
                    }
                    workIterator = workArray;
                    for(auto iter = frontPtr; iter != (backPtr + 1); ++iter)
                    {
                        new(workIterator) value_type(std::move(*iter));
                        (*iter).~value_type();
                        ++workIterator;
                    }

                    allocator.deallocate(beginArray, capacity);
                    beginArray = workArray;
                    endArray = beginArray + newCapacity;
                    frontPtr = beginArray;
                    backPtr = frontPtr + queueSize - 1;

                }
            }

            if (queueSize == 0)
            {
                try
                {
                    new(frontPtr) value_type(std::forward<Args>(args)...);
                    //TYPE(std::forward<Args>(args)...)
                } catch (...)
                {
                    queueSize = 0;
                    backPtr = frontPtr;
                    return;
                }

                backPtr = frontPtr;
                queueSize = 1;
            } else
            {
                try
                {
                    ++backPtr;
                    new(backPtr) value_type(std::forward<Args>(args)...);
                } catch (...)
                {
                    --backPtr;
                    return;
                }
                ++queueSize;
            }
        }

        [[nodiscard]] bool operator == (const VectorQueue<TYPE, ALLOCATOR>& other)
        {
            bool equal = true;

            if (queueSize == other.queueSize)
            {
                for(size_type idx = 0; idx < queueSize; ++idx)
                {
                    if (frontPtr[idx] != other.frontPtr[idx])
                    {
                        equal = true;
                        break;
                    }
                }
            } else
            {
                equal = false;
            }

            return equal;
        }

        [[nodiscard]] bool operator != (const VectorQueue<TYPE, ALLOCATOR>& other)
        {
            bool equal = false;

            if (queueSize == other.queueSize)
            {
                for(std::uint64_t idx = 0; idx < queueSize; ++idx)
                {
                    if (frontPtr[idx] != other.frontPtr[idx])
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

        [[nodiscard]] bool operator >= (const VectorQueue<TYPE, ALLOCATOR>& other)
        {
            bool equal;

            if (queueSize == other.queueSize)
            {
                equal = true;
                for(std::uint64_t idx = 0; idx < queueSize; ++idx)
                {
                    if (frontPtr[idx] != other.frontPtr[idx])
                    {
                        if (frontPtr[idx] > other.frontPtr[idx])
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                }
            } else if (queueSize > other.queueSize)
            {
                equal = true;
                for(std::uint64_t idx = 0; idx < other.queueSize; ++idx)
                {
                    if (frontPtr[idx] != other.frontPtr[idx])
                    {
                        if (frontPtr[idx] > other.frontPtr[idx])
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
                for(std::uint64_t idx = 0; idx < queueSize; ++idx)
                {
                    if (frontPtr[idx] != other.frontPtr[idx])
                    {
                        if (frontPtr[idx] > other.frontPtr[idx])
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

        [[nodiscard]] bool operator > (const VectorQueue<TYPE, ALLOCATOR>& other)
        {
            bool equal;

            if (queueSize == other.queueSize)
            {
                equal = false;
                for(std::uint64_t idx = 0; idx < queueSize; ++idx)
                {
                    if (frontPtr[idx] != other.frontPtr[idx])
                    {
                        if (frontPtr[idx] > other.frontPtr[idx])
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                }
            } else if (queueSize > other.queueSize)
            {
                equal = true;
                for(std::uint64_t idx = 0; idx < other.queueSize; ++idx)
                {
                    if (frontPtr[idx] != other.frontPtr[idx])
                    {
                        if (frontPtr[idx] > other.frontPtr[idx])
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
                for(std::uint64_t idx = 0; idx < queueSize; ++idx)
                {
                    if (frontPtr[idx] != other.frontPtr[idx])
                    {
                        if (frontPtr[idx] > other.frontPtr[idx])
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

        [[nodiscard]] bool operator <= (const VectorQueue<TYPE, ALLOCATOR>& other)
        {
            bool equal;

            if (queueSize == other.queueSize)
            {
                equal = true;
                for(std::uint64_t idx = 0; idx < queueSize; ++idx)
                {
                    if (frontPtr[idx] != other.frontPtr[idx])
                    {
                        if (frontPtr[idx] < other.frontPtr[idx])
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                }
            } else if (queueSize > other.queueSize)
            {
                equal = false;
                for(std::uint64_t idx = 0; idx < other.queueSize; ++idx)
                {
                    if (frontPtr[idx] !=other.frontPtr[idx])
                    {
                        if (frontPtr[idx] < other.frontPtr[idx])
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
                for(std::uint64_t idx = 0; idx < queueSize; ++idx)
                {
                    if (frontPtr[idx] != other.frontPtr[idx])
                    {
                        if (frontPtr[idx] < other.frontPtr[idx])
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

        [[nodiscard]] bool operator < (const VectorQueue<TYPE, ALLOCATOR>& other)
        {
            bool equal;

            if (queueSize == other.queueSize)
            {
                equal = false;
                for(std::uint64_t idx = 0; idx < queueSize; ++idx)
                {
                    if (frontPtr[idx] != other.frontPtr[idx])
                    {
                        if (frontPtr[idx] < other.frontPtr[idx])
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                }
            } else if (queueSize > other.queueSize)
            {
                equal = false;
                for(std::uint64_t idx = 0; idx < other.queueSize; ++idx)
                {
                    if (frontPtr[idx] != other.frontPtr[idx])
                    {
                        if (frontPtr[idx] < other.frontPtr[idx])
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
                for(std::uint64_t idx = 0; idx < queueSize; ++idx)
                {
                    if (frontPtr[idx] != other.frontPtr[idx])
                    {
                        if (frontPtr[idx] < other.frontPtr[idx])
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
        size_type queueSize;
        pointer beginArray;
        pointer endArray;
        pointer frontPtr;
        pointer backPtr;
    };

}

#endif //THREADPOOL_VECTOR_QUEUE_H
