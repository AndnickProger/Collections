#ifndef THREADPOOL_BLOCKING_QUEUE_CONTAINER_H
#define THREADPOOL_BLOCKING_QUEUE_CONTAINER_H

#include <thread>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace th_pool
{
    template<typename TYPE, typename CONTAINER = std::queue<TYPE>>
    class BlockingQueueContainer
    {
    public:

        using container_type              = CONTAINER;
        using value_type                  = typename CONTAINER::value_type;
        using size_type                   = typename CONTAINER::size_type;
        using reference                   = typename CONTAINER::reference;
        using const_reference             = typename CONTAINER::const_reference;
        using container_reference         = CONTAINER&;
        using mutex_type                  = std::mutex;
        using mutex_reference             = std::mutex&;
        using shared_mutex_type           = std::shared_mutex;
        using condition_variable_type     = std::condition_variable;

        BlockingQueueContainer() :
        container()
        {}

        explicit BlockingQueueContainer(container_type container_) :
        container(container_)
        {}

        BlockingQueueContainer(const_reference value) :
        container()
        {
            container.push(value);
        }

        BlockingQueueContainer(std::initializer_list<value_type> list) :
                               container()
        {
            for(auto iter = list.begin(); iter != list.end(); ++iter)
            {
                container.push(*iter);
            }
        }

        template<typename ITERATOR>
        BlockingQueueContainer(ITERATOR begin, ITERATOR end) :
                container()
        {
            for(auto iter = begin; iter != end; ++iter)
            {
                container.push(*iter);
            }
        }

        BlockingQueueContainer(const BlockingQueueContainer<TYPE, CONTAINER>& other)
        {
            std::lock_guard lockGuard(workMutex);
            container = other.container;
        }

        BlockingQueueContainer<TYPE, CONTAINER>& operator = (const BlockingQueueContainer<TYPE, CONTAINER>& other)
        {
            std::lock_guard lockGuard(workMutex);
            container = other.container;
            return *this;
        }

        BlockingQueueContainer(BlockingQueueContainer<TYPE, CONTAINER>&& other) noexcept
        {
            std::lock_guard lockGuard(workMutex);
            container = std::move(container);
        }

        BlockingQueueContainer<TYPE, CONTAINER>& operator = (BlockingQueueContainer<TYPE, CONTAINER>&& other) noexcept
        {
            std::lock_guard lockGuard(workMutex);
            container = std::move(container);
            return *this;
        }

        virtual ~BlockingQueueContainer() = default;

        void lock()
        {
            containerMutex.lock();
        }

        void unlock()
        {
            containerMutex.unlock();
        }

        mutex_reference getContainerMutex()
        {
            std::lock_guard lockGuard(workMutex);
            return containerMutex;
        }

        [[nodiscard]] inline size_type size() const noexcept
        {
            std::lock_guard lockGuard(workMutex);
            return container.size();
        }

        [[nodiscard]] inline bool empty() const noexcept
        {
            std::lock_guard lockGuard(workMutex);
            return container.empty();
        }

        [[nodiscard]] inline reference front()
        {
            std::lock_guard lockGuard(workMutex);
            return container.front();
        }

        [[nodiscard]] inline const_reference front() const
        {
            std::lock_guard lockGuard(workMutex);
            return container.front();
        }

        [[nodiscard]] inline reference back()
        {
            std::lock_guard lockGuard(workMutex);
            return container.back();
        }

        [[nodiscard]] inline const_reference back() const
        {
            std::lock_guard lockGuard(workMutex);
            return container.back();
        }

        void push(const_reference value)
        {
            std::lock_guard lockGuard(workMutex);
            container.push(value);
            emptyCondVar.notify_one();
        }

        void push(value_type&& value)
        {
            std::lock_guard lockGuard(workMutex);
            container.push(std::move(value));
            emptyCondVar.notify_one();
        }

        void pop()
        {
            std::lock_guard lockGuard(workMutex);
            container.pop();
        }

        template<typename... Args>
        void emplace(Args... args)
        {
            std::lock_guard lockGuard(workMutex);
            container.emplace(args...);
            emptyCondVar.notify_one();
        }

        void try_pop(reference element)
        {
            std::lock_guard lockGuard(workMutex);
            if (container.empty())
            {
                throw std::out_of_range("Queue is empty");
            }
            element = std::move(container.front());
            container.pop();
        }

        void wait_and_pop(reference element)
        {
            std::unique_lock uniqueLock(workMutex);
            emptyCondVar.wait(uniqueLock, [this]{return !container.empty();});
            try
            {
                element = std::move(container.front());
                container.pop();
            } catch (...)
            {
                emptyCondVar.notify_one();
                throw;
            }
        }

        container_reference get_container()
        {
            std::lock_guard lockGuard(containerMutex);
            return container;
        }

        [[nodiscard]] inline value_type get_front()
        {
            std::shared_lock sharedLock(sharedMutex);
            return container.front();
        }

    private:

        container_type container;
        mutable mutex_type workMutex;
        mutex_type containerMutex;
        shared_mutex_type sharedMutex;
        std::condition_variable emptyCondVar;
    };
}

#endif //THREADPOOL_BLOCKING_QUEUE_CONTAINER_H
