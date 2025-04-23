#ifndef THREADPOOL_BUFFER_ARRAY_QUEUE_H
#define THREADPOOL_BUFFER_ARRAY_QUEUE_H

#include <stdexcept>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <iostream>
#include <memory>

namespace th_pool
{
    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    class BufferArrayQueue
    {
    public:

        using value_type                 = TYPE;
        using size_type                  = std::uint64_t;
        using difference_type            = std::int64_t;
        using allocator_type             = ALLOCATOR;
        using reference                  = TYPE&;
        using const_reference            = const TYPE&;
        using pointer                    = TYPE*;
        using const_pointer              = const TYPE*;

        BufferArrayQueue(allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        beginArray{}, endArray{},
        frontPtr{}, backPtr{}
        {}

        explicit BufferArrayQueue(const_reference value, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        beginArray{}, endArray{},
        frontPtr{}, backPtr{}
        {
            pointer workArray{nullptr};
            try
            {
                workArray = allocator.allocate(1);
                new(workArray) value_type(value);
            } catch (std::bad_alloc)
            {
                workArray = nullptr;
                return;
            } catch (...)
            {
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

        BufferArrayQueue(std::initializer_list<value_type> list, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        beginArray{}, endArray{},
        frontPtr{}, backPtr{}
        {
            pointer workArray{nullptr};
            pointer workIterator{nullptr};

            try
            {
                workArray = allocator.allocate(list.size());
                workIterator = workArray;
                for (auto iter = list.begin(); iter != list.end(); ++iter)
                {
                    new(workIterator) value_type(*iter);
                    ++workIterator;
                }

            } catch (std::bad_alloc)
            {
                workArray = nullptr;
                return;
            } catch (...)
            {
                for (auto iter = workArray; iter != workIterator; ++iter)
                {
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
        BufferArrayQueue(ITERATOR begin, ITERATOR end, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        beginArray{}, endArray{},
        frontPtr{}, backPtr{}
        {
            difference_type distance{0};
            for (auto iter = begin; iter != end; ++iter)
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
                    for (auto iter = begin; iter != end; ++iter)
                    {
                        new(workIterator) value_type(*iter);
                        ++workIterator;
                    }
                } catch (std::bad_alloc)
                {
                    workArray = nullptr;
                    return;
                } catch (...)
                {
                    for (auto iter = workArray; iter != workIterator; ++iter)
                    {
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
            } else
            {
                throw std::out_of_range("The distance between iterators must be positive");
            }
        }

        BufferArrayQueue(const BufferArrayQueue<TYPE, ALLOCATOR>& other)
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

                    if (other.backPtr < other.frontPtr)
                    {
                        pointer begin = other.frontPtr;
                        pointer end = other.endArray;
                        for(auto iter = begin; iter != end; ++iter)
                        {
                            new(workIterator) value_type(*iter);
                            ++workIterator;
                        }
                        begin = other.beginArray;
                        end = other.backPtr + 1;
                        for(auto iter = begin; iter != end; ++iter)
                        {
                            new(workIterator) value_type(*iter);
                            ++workIterator;
                        }

                    } else
                    {
                        pointer begin = other.frontPtr;
                        pointer end = other.backPtr + 1;

                        for(auto iter = begin; iter != end; ++iter)
                        {
                            new(workIterator) value_type (*iter);
                            ++workIterator;
                        }
                    }

                } catch (std::bad_alloc)
                {
                    workArray = nullptr;
                    return;
                } catch (...)
                {
                    for(auto iter = workArray; iter != workIterator; ++iter)
                    {
                        (*iter).~value_type();
                    }
                    allocator.deallocate(workArray, queueSize);
                    workArray = nullptr;
                    return;
                }

                beginArray = workArray;
                endArray = beginArray + queueSize;
                frontPtr = beginArray;
                backPtr = endArray - 1;

            } else
            {
                beginArray = nullptr;
                endArray = nullptr;
                frontPtr = nullptr;
                backPtr = nullptr;
            }
        }

        BufferArrayQueue<TYPE, ALLOCATOR>& operator = (const BufferArrayQueue<TYPE, ALLOCATOR>& other)
        {
            if (queueSize == other.queueSize)
            {
                pointer workIterator = frontPtr;
                pointer begin;
                pointer end;

                try
                {
                    if (other.beginArray < other.frontPtr)
                    {
                        begin = other.frontPtr;
                        end = other.endArray;
                        for(auto iter = begin; iter != end; ++iter)
                        {
                            *workIterator = *iter;
                            if (workIterator == endArray - 1)
                            {
                                workIterator = beginArray;
                            } else
                            {
                                ++workIterator;
                            }
                        }

                        begin = other.beginArray;
                        end = other.backPtr + 1;
                        for(auto iter = begin; iter != end; ++iter)
                        {
                            *workIterator = *iter;
                            if (workIterator == endArray - 1)
                            {
                                workIterator = beginArray;
                            } else
                            {
                                ++workIterator;
                            }
                        }

                    } else
                    {
                        begin = other.frontPtr;
                        end = other.backPtr + 1;

                        for(auto iter = begin; iter != end; ++iter)
                        {
                            *workIterator = *iter;
                            if (workIterator == endArray - 1)
                            {
                                workIterator = beginArray;
                            } else
                            {
                                ++workIterator;
                            }
                        }
                    }
                } catch (...)
                {
                    if (queueSize > 0)
                    {
                        if (workIterator < frontPtr)
                        {
                            for(auto iter = frontPtr; iter != endArray; ++iter)
                            {
                                (*iter).~value_type();
                            }

                            for(auto iter = beginArray; iter != (backPtr + 1); ++iter)
                            {
                                (*iter).~value_type();
                            }

                        } else
                        {
                            for(auto iter = frontPtr; iter != workIterator; ++iter)
                            {
                                (*iter).~value_type();
                            }
                        }
                    }
                    queueSize = 0;
                    frontPtr = beginArray;
                    backPtr = frontPtr;
                    workIterator = nullptr;
                    return *this;
                }
            } else
            {
                if (beginArray != nullptr)
                {
                    if (queueSize > 0)
                    {
                        if (backPtr < frontPtr)
                        {
                            for(auto iter = beginArray; iter != (backPtr + 1); ++iter)
                            {
                                (*iter).~value_type();
                            }

                            for(auto iter = frontPtr; iter != endArray; ++iter)
                            {
                                (*iter).~value_type();
                            }

                        } else
                        {
                            for(auto iter = frontPtr; iter != (backPtr + 1); ++iter)
                            {
                                (*iter).~value_type();
                            }
                        }
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

                        if (other.backPtr < other.frontPtr)
                        {
                            pointer begin = other.frontPtr;
                            pointer end = other.endArray;
                            for(auto iter = begin; iter != end; ++iter)
                            {
                                new(workIterator) value_type(*iter);
                                //*workIterator = *iter;
                                ++workIterator;
                            }
                            begin = other.beginArray;
                            end = other.backPtr + 1;
                            for(auto iter = begin; iter != end; ++iter)
                            {
                                new(workIterator) value_type(*iter);
                                //*workIterator = *iter;
                                ++workIterator;
                            }

                        } else
                        {
                            pointer begin = other.frontPtr;
                            pointer end = other.backPtr + 1;

                            for(auto iter = begin; iter != end; ++iter)
                            {
                                new(workIterator) value_type(*iter);
                                //*workIterator = *iter;
                                ++workIterator;
                            }
                        }

                    } catch (std::bad_alloc)
                    {
                        workArray = nullptr;
                        return *this;
                    } catch (...)
                    {
                        for(auto iter = workArray; iter != workIterator; ++iter)
                        {
                            (*iter).~value_type();
                        }
                        allocator.deallocate(workArray, queueSize);
                        workArray = nullptr;
                        return *this;
                    }

                    beginArray = workArray;
                    endArray = beginArray + queueSize;
                    frontPtr = beginArray;
                    backPtr = endArray - 1;

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

        BufferArrayQueue(BufferArrayQueue<TYPE, ALLOCATOR>&& other) noexcept :
        queueSize(std::move(other.queueSize)),
        beginArray(std::move(other.beginArray)),
        endArray(std::move(other.endArray)),
        frontPtr(std::move(other.frontPtr)),
        backPtr(std::move(other.backPtr))
        {
            other.queueSize = 0;
            other.beginArray = nullptr;
            other.endArray = nullptr;
            other.frontPtr = nullptr;
            other.backPtr = nullptr;
        }

        BufferArrayQueue<TYPE, ALLOCATOR>& operator = (BufferArrayQueue<TYPE, ALLOCATOR>&& other) noexcept
        {
            if (beginArray != nullptr)
            {
                if (queueSize > 0)
                {
                    if (backPtr < frontPtr)
                    {
                        for(auto iter = beginArray; iter != (backPtr + 1); ++iter)
                        {
                            (*iter).~value_type();
                        }

                        for(auto iter = frontPtr; iter != endArray; ++iter)
                        {
                            (*iter).~value_type();
                        }

                    } else
                    {
                        for(auto iter = frontPtr; iter != (backPtr + 1); ++iter)
                        {
                            (*iter).~value_type();
                        }
                    }
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
            other.beginArray = nullptr;

            return *this;
        }

        virtual ~BufferArrayQueue()
        {
            if (beginArray != nullptr)
            {
                if (queueSize > 0)
                {
                    if (backPtr < frontPtr)
                    {
                        for(auto iter = beginArray; iter != (backPtr + 1); ++iter)
                        {
                            (*iter).~value_type();
                        }

                        for(auto iter = frontPtr; iter != endArray; ++iter)
                        {
                            (*iter).~value_type();
                        }

                    } else
                    {
                        for(auto iter = frontPtr; iter != (backPtr + 1); ++iter)
                        {
                            (*iter).~value_type();
                        }
                    }
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
                pointer workArray{};
                try
                {
                    workArray = allocator.allocate(2);
                    new(workArray) value_type (value);
                } catch (std::bad_alloc)
                {
                    workArray = nullptr;
                    return;
                } catch (...)
                {
                    (*workArray).~value_type();
                    allocator.deallocate(workArray, 2);
                    workArray = nullptr;
                    return;
                }
                queueSize = 1;
                beginArray = workArray;
                endArray = beginArray + 2;
                frontPtr = beginArray;
                backPtr = frontPtr;
                return;
            }

            const difference_type capacity = endArray - beginArray;

            if (capacity == queueSize)
            {
                const difference_type newCapacity = capacity * 2;
                pointer workArray{nullptr};
                pointer workIterator{nullptr};
                try
                {
                    workArray = allocator.allocate(newCapacity);
                    workIterator = workArray;
                    if(backPtr < frontPtr)
                    {
                        for(auto iter = frontPtr; iter != endArray; ++iter)
                        {
                            //*workIterator = *iter;
                            new(workIterator) value_type (std::move(*iter));
                            ++workIterator;
                        }
                        for(auto iter = beginArray; iter != (backPtr + 1); ++iter)
                        {
                            //*workIterator = *iter;
                            new(workIterator) value_type (std::move(*iter));
                            ++workIterator;
                        }
                    } else
                    {
                        for(auto iter = frontPtr; iter != (backPtr + 1); ++iter)
                        {
                            //*workIterator = *iter;
                            new(workIterator) value_type (std::move(*iter));
                            ++workIterator;
                        }
                    }
                } catch (std::bad_alloc)
                {
                    workArray = nullptr;
                    return;
                } catch (...)
                {
                    for(auto iter = workArray; iter != workIterator; ++iter)
                    {
                        (*iter).~value_type();
                    }
                    allocator.deallocate(workArray, newCapacity);
                    workArray = nullptr;
                    return;
                }

                allocator.deallocate(beginArray, capacity);
                beginArray = workArray;
                endArray = beginArray + newCapacity;
                frontPtr = beginArray;
                backPtr = frontPtr + queueSize - 1;
            }

            if (backPtr + 1 == endArray)
            {
                try
                {
                    backPtr = beginArray;
                    //*backPtr = value;
                    new(backPtr) value_type (value);
                    ++queueSize;
                } catch (...)
                {
                    backPtr = endArray - 1;
                    --queueSize;
                    return;
                }
            } else
            {
                try
                {
                    ++backPtr;
                    //*backPtr = value;
                    new(backPtr) value_type (value);
                    ++queueSize;
                } catch (...)
                {
                    --backPtr;
                    --queueSize;
                    return;
                }
            }
        }

        void push(value_type&& value)
        {
            if (queueSize == 0 && beginArray == nullptr)
            {
                pointer workArray{};
                try
                {
                    workArray = allocator.allocate(2);
                    new(workArray) value_type (value);
                } catch (std::bad_alloc)
                {
                    workArray = nullptr;
                    return;
                } catch (...)
                {
                    (*workArray).~value_type();
                    allocator.deallocate(workArray, 2);
                    workArray = nullptr;
                    return;
                }
                queueSize = 1;
                beginArray = workArray;
                endArray = beginArray + 2;
                frontPtr = beginArray;
                backPtr = frontPtr;
                return;
            }

            const difference_type capacity = endArray - beginArray;

            if (capacity == queueSize)
            {
                const difference_type newCapacity = capacity * 2;
                pointer workArray{nullptr};
                pointer workIterator{nullptr};
                try
                {
                    workArray = allocator.allocate(newCapacity);
                    workIterator = workArray;
                    if(backPtr < frontPtr)
                    {
                        for(auto iter = frontPtr; iter != endArray; ++iter)
                        {
                            //*workIterator = *iter;
                            new(workIterator) value_type (std::move(*iter));
                            ++workIterator;
                        }
                        for(auto iter = beginArray; iter != (backPtr + 1); ++iter)
                        {
                            //*workIterator = *iter;
                            new(workIterator) value_type (std::move(*iter));
                            ++workIterator;
                        }
                    } else
                    {
                        for(auto iter = frontPtr; iter != (backPtr + 1); ++iter)
                        {
                            //*workIterator = *iter;
                            new(workIterator) value_type (std::move(*iter));
                            ++workIterator;
                        }
                    }
                } catch (std::bad_alloc)
                {
                    workArray = nullptr;
                    return;
                } catch (...)
                {
                    for(auto iter = workArray; iter != workIterator; ++iter)
                    {
                        (*iter).~value_type();
                    }
                    allocator.deallocate(workArray, newCapacity);
                    workArray = nullptr;
                    return;
                }

                allocator.deallocate(beginArray, capacity);
                beginArray = workArray;
                endArray = beginArray + newCapacity;
                frontPtr = beginArray;
                backPtr = frontPtr + queueSize - 1;
            }

            if (backPtr + 1 == endArray)
            {
                backPtr = beginArray;
                //*backPtr = value;
                new(backPtr) value_type (std::move(value));
                ++queueSize;

            } else
            {
                ++backPtr;
                //*backPtr = value;
                new(backPtr) value_type (std::move(value));
                ++queueSize;
            }
        }

        void pop()
        {
            if (queueSize > 0)
            {
                if (queueSize == 1)
                {
                    (*frontPtr).~TYPE();
                    queueSize = 0;
                    frontPtr = beginArray;
                    backPtr = endArray;
                } else
                {
                    if (frontPtr + 1 == endArray)
                    {
                        (*frontPtr).~TYPE();
                        frontPtr = beginArray;
                        --queueSize;

                    } else
                    {
                        (*frontPtr).~TYPE();
                        ++frontPtr;
                        --queueSize;
                    }
                }
            }
        }

        template<typename... Args>
        void emplace(Args... args)
        {
            if (queueSize == 0 && beginArray == nullptr)
            {
                pointer workArray{};
                try
                {
                    workArray = allocator.allocate(2);
                    new(workArray) value_type (std::forward<Args>(args)...);
                } catch (std::bad_alloc)
                {
                    workArray = nullptr;
                    return;
                } catch (...)
                {
                    (*workArray).~value_type();
                    allocator.deallocate(workArray, 2);
                    workArray = nullptr;
                    return;
                }
                queueSize = 1;
                beginArray = workArray;
                endArray = beginArray + 2;
                frontPtr = beginArray;
                backPtr = frontPtr;
                return;
            }

            const difference_type capacity = endArray - beginArray;

            if (capacity == queueSize)
            {
                const difference_type newCapacity = capacity * 2;
                pointer workArray{nullptr};
                pointer workIterator{nullptr};
                try
                {
                    workArray = allocator.allocate(newCapacity);
                    workIterator = workArray;
                    if(backPtr < frontPtr)
                    {
                        for(auto iter = frontPtr; iter != endArray; ++iter)
                        {
                            //*workIterator = *iter;
                            new(workIterator) value_type (std::move(*iter));
                            ++workIterator;
                        }
                        for(auto iter = beginArray; iter != (backPtr + 1); ++iter)
                        {
                            //*workIterator = *iter;
                            new(workIterator) value_type (std::move(*iter));
                            ++workIterator;
                        }
                    } else
                    {
                        for(auto iter = frontPtr; iter != (backPtr + 1); ++iter)
                        {
                            //*workIterator = *iter;
                            new(workIterator) value_type (std::move(*iter));
                            ++workIterator;
                        }
                    }
                } catch (std::bad_alloc)
                {
                    workArray = nullptr;
                    return;
                } catch (...)
                {
                    for(auto iter = workArray; iter != workIterator; ++iter)
                    {
                        (*iter).~value_type();
                    }
                    allocator.deallocate(workArray, newCapacity);
                    workArray = nullptr;
                    return;
                }

                allocator.deallocate(beginArray, capacity);
                beginArray = workArray;
                endArray = beginArray + newCapacity;
                frontPtr = beginArray;
                backPtr = frontPtr + queueSize - 1;
            }

            if (backPtr + 1 == endArray)
            {
                try
                {
                    backPtr = beginArray;
                    //*backPtr = value;
                    new(backPtr) value_type (std::forward<Args>(args)...);
                    ++queueSize;
                } catch (...)
                {
                    backPtr = endArray - 1;
                    --queueSize;
                    return;
                }
            } else
            {
                try
                {
                    ++backPtr;
                    //*backPtr = value;
                    new(backPtr) value_type (std::forward<Args>(args)...);
                    ++queueSize;
                } catch (...)
                {
                    --backPtr;
                    --queueSize;
                    return;
                }
            }
        }

        void swap(BufferArrayQueue<TYPE, ALLOCATOR>& other)
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

        [[nodiscard]] inline bool operator == (const BufferArrayQueue<TYPE, ALLOCATOR>& other)
        {
            bool equal{true};

            if (queueSize == other.queueSize)
            {
                pointer left = frontPtr;
                pointer right = other.frontPtr;

                for(size_type idx = 0; idx < queueSize; ++idx)
                {
                    if (*left != *right)
                    {
                        equal = false;
                        break;
                    }

                    if (left == endArray - 1)
                    {
                        left = beginArray;
                    } else
                    {
                        ++left;
                    }

                    if (right == other.endArray - 1)
                    {
                        right = other.beginArray;
                    } else
                    {
                        ++right;
                    }
                }

            } else
            {
                equal = false;
            }

            return equal;
        }

        [[nodiscard]] inline bool operator != (const BufferArrayQueue<TYPE, ALLOCATOR>& other)
        {
            bool equal{false};

            if (queueSize == other.queueSize)
            {
                pointer left = frontPtr;
                pointer right = other.frontPtr;

                for(size_type idx = 0; idx < queueSize; ++idx)
                {
                    if (*left != *right)
                    {
                        equal = true;
                        break;
                    }

                    if (left == endArray - 1)
                    {
                        left = beginArray;
                    } else
                    {
                        ++left;
                    }

                    if (right == other.endArray - 1)
                    {
                        right = other.beginArray;
                    } else
                    {
                        ++right;
                    }
                }

            } else
            {
                equal = true;
            }

            return equal;
        }

        [[nodiscard]] inline bool operator >= (const BufferArrayQueue<TYPE, ALLOCATOR>& other)
        {
            bool equal;

            if (queueSize == other.queueSize)
            {
                equal = true;
                pointer left = frontPtr;
                pointer right = other.frontPtr;
                for(size_type idx = 0; idx < queueSize; ++idx)
                {
                    if (*left != *right)
                    {
                        if (*left > *right)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }

                    if (left == endArray - 1)
                    {
                        left = beginArray;
                    } else
                    {
                        ++left;
                    }

                    if (right == other.endArray - 1)
                    {
                        right = other.beginArray;
                    } else
                    {
                        ++right;
                    }
                }

            } else if (queueSize > other.queueSize)
            {
                equal = true;
                pointer left = frontPtr;
                pointer right = other.frontPtr;

                for(size_type idx = 0; idx < other.queueSize; ++idx)
                {
                    if (*left != *right)
                    {
                        if (*left > *right)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }

                    if (left == endArray - 1)
                    {
                        left = beginArray;
                    } else
                    {
                        ++left;
                    }

                    if (right == other.endArray - 1)
                    {
                        right = other.beginArray;
                    } else
                    {
                        ++right;
                    }
                }
            } else
            {
                equal = false;
                pointer left = frontPtr;
                pointer right = other.frontPtr;

                for(size_type idx = 0; idx < queueSize; ++idx)
                {
                    if (*left != *right)
                    {
                        if (*left > *right)
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }

                    if (left == endArray - 1)
                    {
                        left = beginArray;
                    } else
                    {
                        ++left;
                    }

                    if (right == other.endArray - 1)
                    {
                        right = other.beginArray;
                    } else
                    {
                        ++right;
                    }
                }

            }

            return equal;
        }

        [[nodiscard]] inline bool operator > (const BufferArrayQueue<TYPE, ALLOCATOR>& other)
        {
            bool equal;

            if (queueSize == other.queueSize)
            {
                equal = false;
                pointer left = frontPtr;
                pointer right = other.frontPtr;
                for(size_type idx = 0; idx < queueSize; ++idx)
                {
                    if (*left != *right)
                    {
                        if (*left > *right)
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }

                    if (left == endArray - 1)
                    {
                        left = beginArray;
                    } else
                    {
                        ++left;
                    }

                    if (right == other.endArray - 1)
                    {
                        right = other.beginArray;
                    } else
                    {
                        ++right;
                    }
                }

            } else if (queueSize > other.queueSize)
            {
                equal = true;
                pointer left = frontPtr;
                pointer right = other.frontPtr;
                for(size_type idx = 0; idx < other.queueSize; ++idx)
                {
                    if (*left != *right)
                    {
                        if (*left > *right)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                    if (left == endArray - 1)
                    {
                        left = beginArray;
                    } else
                    {
                        ++left;
                    }

                    if (right == other.endArray - 1)
                    {
                        right = other.beginArray;
                    } else
                    {
                        ++right;
                    }
                }
            } else
            {
                equal = false;
                pointer left = frontPtr;
                pointer right = other.frontPtr;
                for(size_type idx = 0; idx < queueSize; ++idx)
                {
                    if (*left != *right)
                    {
                        equal = true;
                        break;
                    } else
                    {
                        break;
                    }

                    if (left == endArray - 1)
                    {
                        left = beginArray;
                    } else
                    {
                        ++left;
                    }

                    if (right == other.endArray - 1)
                    {
                        right = other.beginArray;
                    } else
                    {
                        ++right;
                    }
                }
            }

            return equal;
        }

        [[nodiscard]] inline bool operator <= (const BufferArrayQueue<TYPE, ALLOCATOR>& other)
        {
            bool equal;

            if (queueSize == other.queueSize)
            {
                pointer left = frontPtr;
                pointer right = other.frontPtr;
                for(size_type idx = 0; idx < queueSize; ++idx)
                {
                    if (*left != *right)
                    {
                        if (*left < *right)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }

                    if (left == endArray - 1)
                    {
                        left = beginArray;
                    } else
                    {
                        ++left;
                    }

                    if (right == other.endArray - 1)
                    {
                        right = other.beginArray;
                    } else
                    {
                        ++right;
                    }
                }
            } else if (queueSize > other.queueSize)
            {
                equal = false;
                pointer left = frontPtr;
                pointer right = other.frontPtr;
                for(size_type idx = 0; idx < other.queueSize; ++idx)
                {
                    if (*left != *right)
                    {
                        if (*left < *right)
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }

                    if (left == endArray - 1)
                    {
                        left = beginArray;
                    } else
                    {
                        ++left;
                    }

                    if (right == other.endArray - 1)
                    {
                        right = other.beginArray;
                    } else
                    {
                        ++right;
                    }
                }

            } else
            {
                equal = true;
                pointer left = frontPtr;
                pointer right = other.frontPtr;
                for(size_type idx = 0; idx < queueSize; ++idx)
                {
                    if (*left != *right)
                    {
                        if (*left < *right)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }

                    if (left == endArray - 1)
                    {
                        left = beginArray;
                    } else
                    {
                        ++left;
                    }

                    if (right == other.endArray - 1)
                    {
                        right = other.beginArray;
                    } else
                    {
                        ++right;
                    }
                }
            }

            return equal;
        }

        [[nodiscard]] inline bool operator < (const BufferArrayQueue<TYPE, ALLOCATOR>& other)
        {
            bool equal;

            if (queueSize == other.queueSize)
            {
                equal = false;
                pointer left = frontPtr;
                pointer right = other.frontPtr;
                for(size_type idx = 0; idx < queueSize; ++idx)
                {
                    if (*left != *right)
                    {
                        if (*left < *right)
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }

                    if (left == endArray - 1)
                    {
                        left = beginArray;
                    } else
                    {
                        ++left;
                    }

                    if (right == other.endArray - 1)
                    {
                        right = other.beginArray;
                    } else
                    {
                        ++right;
                    }
                }
            } else if (queueSize > other.queueSize)
            {
                equal = false;
                pointer left = frontPtr;
                pointer right = other.frontPtr;
                for(size_type idx = 0; idx < other.queueSize; ++idx)
                {
                    if (*left != *right)
                    {
                        if (*left < *right)
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }

                    if (left == endArray - 1)
                    {
                        left = beginArray;
                    } else
                    {
                        ++left;
                    }

                    if (right == other.endArray - 1)
                    {
                        right = other.beginArray;
                    } else
                    {
                        ++right;
                    }
                }
            } else
            {
                equal = true;
                pointer left = frontPtr;
                pointer right = other.frontPtr;
                for(size_type idx = 0; idx < queueSize; ++idx)
                {
                    if (*left != *right)
                    {
                        if (*left < *right)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }

                    if (left == endArray - 1)
                    {
                        left = beginArray;
                    } else
                    {
                        ++left;
                    }

                    if (right == other.endArray - 1)
                    {
                        right = other.beginArray;
                    } else
                    {
                        ++right;
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

#endif //THREADPOOL_BUFFER_ARRAY_QUEUE_H
