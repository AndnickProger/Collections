#ifndef THREADPOOL_BLOCKING_QUEUE_H
#define THREADPOOL_BLOCKING_QUEUE_H

#include <stdexcept>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include <future>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <shared_mutex>

namespace th_pool
{
    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    class BlockingQueue
    {
    public:

        using value_type                        = TYPE;
        using size_type                         = std::uint64_t;
        using difference_type                   = std::int64_t;
        using allocator_type                    = ALLOCATOR;
        using reference                         = TYPE&;
        using const_reference                   = const TYPE&;
        using pointer                           = TYPE*;
        using const_pointer                     = const TYPE*;
        using mutex_type                        = std::mutex;
        using mutex_reference                   = std::mutex&;
        using shared_mutex_type                 = std::shared_mutex;
        using condition_variable_type           = std::condition_variable;

        BlockingQueue(allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        beginArray{}, endArray{},
        frontPtr{}, backPtr{}
        {}

        explicit BlockingQueue(const_reference value, allocator_type allocator_ = ALLOCATOR()) :
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
                allocator.deallocate(workArray, 1);
                workArray = nullptr;
                return;
            }
            queueSize = 1;
            beginArray = workArray;
            endArray = beginArray + 1;
            frontPtr = beginArray;
            backPtr = beginArray;
        }

        BlockingQueue(std::initializer_list<value_type> list, allocator_type allocator_ = ALLOCATOR()) :
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

                for(auto iter = list.begin(); iter != list.end(); ++iter)
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
                for(auto iter = workArray; iter != workIterator; ++iter)
                {
                    (*iter).~value_type();
                }
                allocator.deallocate(workArray, list.size());
                workArray = nullptr;
                workIterator = nullptr;
                return;
            }

            queueSize = list.size();
            beginArray = workArray;
            endArray = beginArray + queueSize;
            frontPtr = beginArray;
            backPtr = endArray - 1;
        }

        template<typename ITERATOR>
        BlockingQueue(const ITERATOR begin, const ITERATOR end, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        beginArray{}, endArray{},
        frontPtr{}, backPtr{}
        {
            difference_type distance{0};
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
                }catch (std::bad_alloc)
                {
                    workArray = nullptr;
                    return;
                } catch (...)
                {
                    for(auto iter = workArray; iter != workIterator; ++iter)
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
            }
        }

        BlockingQueue(BlockingQueue<TYPE, ALLOCATOR>&& other) noexcept
        {
            std::lock_guard lockGuard(workMutex);

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
        }

        BlockingQueue<TYPE, ALLOCATOR>& operator = (BlockingQueue<TYPE, ALLOCATOR>&& other) noexcept
        {
            std::lock_guard lockGuard(workMutex);

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

        virtual ~BlockingQueue()
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
            std::lock_guard lockGuard(workMutex);
            return queueSize == 0;
        }

        [[nodiscard]] inline size_type size() const noexcept
        {
            std::lock_guard lockGuard(workMutex);
            return queueSize;
        }

        [[nodiscard]] inline reference front()
        {
            std::lock_guard lockGuard(workMutex);
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
            std::lock_guard lockGuard(workMutex);
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
            std::lock_guard lockGuard(workMutex);
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
            std::lock_guard lockGuard(workMutex);
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
            std::lock_guard lockGuard(workMutex);

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

            emptyCondVar.notify_one();
        }

        void push(value_type&& value)
        {
            std::lock_guard lockGuard(workMutex);

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
            emptyCondVar.notify_one();
        }

        void pop()
        {
            std::lock_guard lockGuard(workMutex);

            if (queueSize > 0)
            {
                (*frontPtr).~value_type();
                ++frontPtr;
                --queueSize;
            }
        }

        template<typename... Args>
        void emplace(Args... args)
        {
            std::lock_guard lockGuard(workMutex);

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
            emptyCondVar.notify_one();
        }

        void lock()
        {
            queueMutex.lock();
        }

        void unlock()
        {
            queueMutex.unlock();
        }

        [[nodiscard]] inline value_type get_front()
        {
            std::shared_lock sharedLock(sharedMutex);
            if (frontPtr != nullptr)
            {
                return *frontPtr;
            } else
            {
                throw std::out_of_range("Queue is empty");
            }
        }

        [[nodiscard]] inline mutex_reference getQueueMutex() noexcept
        {
            std::lock_guard lockGuard(workMutex);
            return queueMutex;
        }

        bool try_pop(reference element)
        {
            std::lock_guard lockGuard(workMutex);
            if (queueSize == 0)
            {
                return false;
            }
            element = std::move(*frontPtr);
            (*frontPtr).~value_type();
            ++frontPtr;
            --queueSize;
            return true;
        }

        void wait_and_pop(reference element)
        {
            std::unique_lock uniqueLock(workMutex);
            emptyCondVar.wait(uniqueLock, [this]{return queueSize != 0;});
            try
            {
                element = std::move(*frontPtr);
                (*frontPtr).~value_type();
                ++frontPtr;
                --queueSize;
            } catch (...)
            {
                emptyCondVar.notify_one();
                throw;
            }
        }

    private:

        allocator_type allocator;
        size_type queueSize;
        pointer beginArray;
        pointer endArray;
        pointer frontPtr;
        pointer backPtr;
        mutable mutex_type workMutex;
        mutex_type queueMutex;
        shared_mutex_type sharedMutex;
        condition_variable_type emptyCondVar;

        BlockingQueue(const BlockingQueue<TYPE, ALLOCATOR>& other) = delete;

        BlockingQueue<TYPE, ALLOCATOR>& operator = (const BlockingQueue<TYPE, ALLOCATOR>& other) = delete;
    };
}

#endif //THREADPOOL_BLOCKING_QUEUE_H
