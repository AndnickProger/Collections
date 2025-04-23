#ifndef THREADPOOL_SINGLY_LIST_QUEUE_H
#define THREADPOOL_SINGLY_LIST_QUEUE_H

#include <stdexcept>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <iostream>
#include <vector>
#include <utility>

namespace th_pool
{
    template<typename TYPE>
    struct QueueSinglyNode
    {
        using value_type                 = TYPE;
        using size_type                  = std::uint64_t;
        using difference_type            = std::int64_t;
        using reference                  = TYPE&;
        using const_reference            = const TYPE&;
        using pointer                    = TYPE*;
        using const_pointer              = const TYPE*;
        using node_pointer               = QueueSinglyNode<TYPE>*;
        using const_node_pointer         = const QueueSinglyNode<TYPE>*;

        [[nodiscard]] inline bool operator == (const QueueSinglyNode<TYPE>& other) const noexcept
        {
            return value == other.value && next == other.next;
        }

        [[nodiscard]] inline bool operator != (const QueueSinglyNode<TYPE>& other) const noexcept
        {
            return value != other.value || next != other.next;
        }

        value_type value;
        node_pointer next;
    };

    template<typename TYPE>
    std::ostream& operator << (std::ostream& os, const QueueSinglyNode<TYPE>& node)
    {
        os << node.value;
        return os;
    }

    /*template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    class SinglyListQueue
    {

    public:

        using value_type                 = TYPE;
        using size_type                  = std::uint64_t;
        using difference_type            = std::int64_t;
        using allocator_type             = ALLOCATOR;
        using real_allocator_type        = typename std::allocator_traits<ALLOCATOR>::template rebind_alloc<QueueSinglyNode<TYPE>>;
        using reference                  = TYPE&;
        using const_reference            = const TYPE&;
        using pointer                    = TYPE*;
        using const_pointer              = const TYPE*;
        using node_pointer               = QueueSinglyNode<TYPE>*;
        using const_node_pointer         = const QueueSinglyNode<TYPE>*;

        SinglyListQueue(ALLOCATOR allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        backNode{}, frontNode{}
        {}

        explicit SinglyListQueue(const_reference value_, ALLOCATOR allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        backNode{}, frontNode{}
        {
            node_pointer workNode{};
            try
            {
                workNode = allocator.allocate(1);
                new(workNode) QueueSinglyNode<TYPE>{value_, nullptr};
            } catch (std::bad_alloc)
            {
                workNode = nullptr;
                return;
            } catch (...)
            {
                (*workNode).~QueueSinglyNode<TYPE>();
                allocator.deallocate(workNode, 1);
                workNode = nullptr;
                return;
            }
            queueSize = 1;
            backNode = workNode;
            frontNode = workNode;
        }

        SinglyListQueue(std::initializer_list<value_type> list, ALLOCATOR allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        backNode{}, frontNode{}
        {
            for(auto iter = list.begin(); iter != list.end(); ++iter)
            {
                push(*iter);
            }
        }

        template<typename ITERATOR>
        SinglyListQueue(ITERATOR begin, ITERATOR end, ALLOCATOR allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        backNode{}, frontNode{}
        {
            difference_type distance{};
            for(auto iter = begin; iter != end; ++iter)
            {
                ++distance;
            }

            if (distance > 0)
            {
                for(auto iter = begin; iter != end; ++iter)
                {
                    push(*iter);
                }
            } else
            {
                throw std::out_of_range("The distance between iterators must be positive");
            }
        }

        SinglyListQueue(const SinglyListQueue<TYPE>& other)
        {
            queueSize = other.queueSize;
            if (queueSize > 0)
            {
                node_pointer otherNode = other.backNode;
                node_pointer previousNode;
                node_pointer workNode;
                try
                {
                    workNode = allocator.allocate(1);
                    //workNode->value = otherNode->value;
                    new(workNode) QueueSinglyNode<TYPE>{value_type (otherNode->value), nullptr};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    queueSize = 0;
                    backNode = nullptr;
                    frontNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueSinglyNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    queueSize = 0;
                    backNode = nullptr;
                    frontNode = nullptr;
                    return;
                }
                workNode->nodeNext = nullptr;
                backNode = workNode;

                while (otherNode->nodeNext != nullptr)
                {
                    otherNode = otherNode->nodeNext;
                    previousNode = workNode;
                    try
                    {
                        workNode = allocator.allocate(1);
                        //workNode->value = otherNode->value;
                        new(workNode) QueueSinglyNode<TYPE>{value_type(otherNode->value), nullptr};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = backNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            currentNode = currentNode->nodeNext;
                            (*deleteNode).~QueueSinglyNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        backNode = nullptr;
                        frontNode = nullptr;
                        queueSize = 0;
                        return;
                    } catch (...)
                    {
                        allocator.deallocate(workNode, 1);
                        node_pointer currentNode = backNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            currentNode = currentNode->nodeNext;
                            (*deleteNode).~QueueSinglyNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        backNode = nullptr;
                        frontNode = nullptr;
                        queueSize = 0;
                        return;

                    }
                    previousNode->nodeNext = workNode;
                    workNode->nodeNext = nullptr;
                }
                frontNode = workNode;

            } else
            {
                backNode = nullptr;
                frontNode = nullptr;
            }
        }

        SinglyListQueue<TYPE>& operator = (const SinglyListQueue<TYPE>& other)
        {
            if (queueSize == other.queueSize)
            {
                node_pointer currentNode = backNode;
                node_pointer deleteNode = currentNode;

                while (currentNode != nullptr)
                {
                    currentNode = currentNode->nodeNext;
                    (*deleteNode).~QueueSinglyNode<TYPE>();
                    deleteNode = currentNode;
                }
                node_pointer otherNode = other.backNode;
                node_pointer workNode = backNode;
                try
                {
                    while (otherNode != nullptr && workNode != nullptr)
                    {
                        workNode->value = otherNode->value;
                        workNode = workNode->nodeNext;
                        otherNode = otherNode->nodeNext;
                    }
                } catch (...)
                {
                    node_pointer currentNode = backNode;
                    node_pointer deleteNode = currentNode;

                    while (currentNode != nullptr)
                    {
                        currentNode = currentNode->nodeNext;
                        (*deleteNode).~QueueSinglyNode<TYPE>();
                        allocator.deallocate(deleteNode, 1);
                        deleteNode = currentNode;
                    }
                    backNode = nullptr;
                    frontNode = nullptr;
                    queueSize = 0;
                    return *this;
                }
            } else
            {
                node_pointer currentNode = backNode;
                node_pointer deleteNode = currentNode;

                while (currentNode != nullptr)
                {
                    currentNode = currentNode->nodeNext;
                    (*deleteNode).~QueueSinglyNode<TYPE>();
                    allocator.deallocate(deleteNode, 1);
                    deleteNode = currentNode;
                }
                backNode = nullptr;
                frontNode = nullptr;

                queueSize = other.queueSize;
                if (queueSize > 0)
                {
                    node_pointer otherNode = other.backNode;
                    node_pointer previousNode;
                    node_pointer workNode;
                    try
                    {
                        workNode = allocator.allocate(1);
                        workNode->value = otherNode->value;
                    } catch (std::bad_alloc)
                    {
                        workNode = nullptr;
                        queueSize = 0;
                        backNode = nullptr;
                        frontNode = nullptr;
                        return *this;
                    }catch (...)
                    {
                        (*workNode).~QueueSinglyNode<TYPE>();
                        allocator.deallocate(workNode, 1);
                        queueSize = 0;
                        backNode = nullptr;
                        frontNode = nullptr;
                        return *this;
                    }
                    workNode->nodeNext = nullptr;
                    backNode = workNode;

                    while (otherNode->nodeNext != nullptr)
                    {
                        otherNode = otherNode->nodeNext;
                        previousNode = workNode;
                        try
                        {
                            workNode = allocator.allocate(1);
                            workNode->value = otherNode->value;
                        } catch (std::bad_alloc)
                        {
                            node_pointer currentNode = backNode;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != nullptr)
                            {
                                currentNode = currentNode->nodeNext;
                                (*deleteNode).~QueueSinglyNode<TYPE>();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }
                            backNode = nullptr;
                            frontNode = nullptr;
                            queueSize = 0;
                            return *this;
                        } catch (...)
                        {
                            allocator.deallocate(workNode, 1);
                            node_pointer currentNode = backNode;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != nullptr)
                            {
                                currentNode = currentNode->nodeNext;
                                (*deleteNode).~QueueSinglyNode<TYPE>();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }
                            backNode = nullptr;
                            frontNode = nullptr;
                            queueSize = 0;
                            return *this;

                        }
                        previousNode->nodeNext = workNode;
                        workNode->nodeNext = nullptr;
                    }
                    frontNode = workNode;

                } else
                {
                    backNode = nullptr;
                    frontNode = nullptr;
                }
            }

            return *this;
        }

        SinglyListQueue(SinglyListQueue<TYPE>&& other) noexcept :
        queueSize(std::move(other.queueSize)),
        backNode(std::move(other.backNode)),
        frontNode(std::move(other.frontNode))
        {
            other.backNode = nullptr;
            other.frontNode = nullptr;
            other.queueSize = {};
        }

        SinglyListQueue<TYPE>& operator = (SinglyListQueue<TYPE>&& other) noexcept
        {
            node_pointer currentNode = backNode;
            node_pointer deleteNode = currentNode;

            while (currentNode != nullptr)
            {
                currentNode = currentNode->nodeNext;
                (*deleteNode).~QueueSinglyNode<TYPE>();
                allocator.deallocate(deleteNode, 1);
                deleteNode = currentNode;
            }
            backNode = nullptr;
            frontNode = nullptr;

            queueSize = std::move(other.queueSize);
            backNode = std::move(other.backNode);
            frontNode = std::move(other.frontNode);

            other.backNode = nullptr;
            other.frontNode = nullptr;
            other.queueSize = {};

            return *this;
        }

        virtual ~SinglyListQueue()
        {
            node_pointer currentNode = backNode;
            node_pointer deleteNode = currentNode;

            while (currentNode != nullptr)
            {
                currentNode = currentNode->nodeNext;
                (*deleteNode).~QueueSinglyNode<TYPE>();
                allocator.deallocate(deleteNode, 1);
                deleteNode = currentNode;
            }
            backNode = nullptr;
            frontNode = nullptr;
            queueSize = {};
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
                return frontNode->value;
            } else
            {
                throw std::out_of_range("Queue is empty");
            }
        }

        [[nodiscard]] inline const_reference front() const
        {
            if (queueSize > 0)
            {
                return frontNode->value;
            } else
            {
                throw std::out_of_range("Queue is empty");
            }
        }

        [[nodiscard]] inline reference back()
        {
            if (queueSize > 0)
            {
                return backNode->value;
            } else
            {
                throw std::out_of_range("Queue is empty");
            }
        }

        [[nodiscard]] inline const_reference back() const
        {
            if (queueSize > 0)
            {
                return backNode->value;
            } else
            {
                throw std::out_of_range("Queue is empty");
            }
        }

        inline void push(const_reference value)
        {
            if (queueSize == 0)
            {
                try
                {
                    backNode = allocator.allocate(1);
                    //backNode->value = value;
                    new(backNode) QueueSinglyNode<TYPE>{value_type(value), nullptr};
                } catch (std::bad_alloc)
                {
                    backNode = nullptr;
                    frontNode = nullptr;
                    queueSize = 0;
                    return;
                } catch (...)
                {
                    (*backNode).~QueueSinglyNode<TYPE>();
                    allocator.deallocate(backNode, 1);
                    backNode = nullptr;
                    frontNode = nullptr;
                    queueSize = 0;
                    return;
                }
                //backNode->nodeNext = nullptr;
                frontNode = backNode;
                queueSize = 1;
            } else if (queueSize == 1)
            {
                node_pointer workNode;
                try
                {
                    workNode = allocator.allocate(1);
                    //workNode->value = value;
                    new(workNode) QueueSinglyNode<TYPE>{value_type(value), frontNode};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                }catch (...)
                {
                    (*backNode).~QueueSinglyNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    return;
                }
                //workNode->nodeNext = frontNode;
                backNode = workNode;
                queueSize = 2;

            } else
            {
                node_pointer workNode{};
                try
                {
                    workNode = allocator.allocate(1);
                    //workNode->value = value;
                    new(workNode) QueueSinglyNode<TYPE>{value_type(value), backNode};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                }catch (...)
                {
                    (*backNode).~QueueSinglyNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                //workNode->nodeNext = backNode;
                backNode = workNode;
                ++queueSize;
            }
        }

        inline void push(value_type&& value)
        {
            if (queueSize == 0)
            {
                try
                {
                    backNode = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    backNode = nullptr;
                    frontNode = nullptr;
                    queueSize = 0;
                    return;
                }
                //backNode->value = std::move(value);
                //backNode->nodeNext = nullptr;
                new(backNode) QueueSinglyNode<TYPE>{value_type(std::move(value)), nullptr};
                frontNode = backNode;
                queueSize = 1;
            } else if (queueSize == 1)
            {
                node_pointer workNode;
                try
                {
                    workNode = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                }
                //workNode->value = std::move(value);
                //workNode->nodeNext = frontNode;
                new(workNode) QueueSinglyNode<TYPE>{value_type(std::move(value)), frontNode};
                backNode = workNode;
                queueSize = 2;

            } else
            {
                node_pointer workNode{};
                try
                {
                    workNode = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                }

                //workNode->value = std::move(value);
                //workNode->nodeNext = backNode;
                new(workNode) QueueSinglyNode<TYPE>{value_type(std::move(value)), backNode};
                backNode = workNode;
                ++queueSize;
            }
        }

        template<typename... Args>
        void emplace(Args... args)
        {
            if (queueSize == 0)
            {
                node_pointer workNode = nullptr;
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) QueueSinglyNode<TYPE>{TYPE(std::forward<Args>(args)...), nullptr};

                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    queueSize = 0;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueSinglyNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    queueSize = 0;
                    return;
                }
                queueSize = 1;
                backNode = workNode;
                frontNode = workNode;

            } else if (queueSize == 1)
            {
                node_pointer workNode = nullptr;
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) QueueSinglyNode<TYPE>{TYPE(std::forward<Args>(args)...), frontNode};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    queueSize = 1;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueSinglyNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    queueSize = 1;
                    return;
                }
                backNode = workNode;
                queueSize = 2;
            } else
            {
                node_pointer workNode = nullptr;
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) QueueSinglyNode<TYPE>{TYPE(std::forward<Args>(args)...), backNode};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueSinglyNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    return;
                }
                backNode = workNode;
                ++queueSize;
            }

        }

        void pop()
        {
            if (queueSize == 1)
            {
                if (backNode != nullptr)
                {
                    (*backNode).~QueueSinglyNode<TYPE>();
                    allocator.deallocate(backNode, 1);
                }
                queueSize = 0;
                backNode = nullptr;
                frontNode = nullptr;
            } else if (queueSize == 2)
            {
                backNode->nodeNext = nullptr;
                queueSize = 1;
                if (frontNode != nullptr)
                {
                    (*frontNode).~QueueSinglyNode<TYPE>();
                    allocator.deallocate(frontNode, 1);
                }
                frontNode = backNode;
            } else if (queueSize > 2)
            {
                node_pointer node = backNode;
                while (node->nodeNext != frontNode)
                {
                    node = node->nodeNext;
                }
                --queueSize;
                if (frontNode != nullptr)
                {
                    (*frontNode).~QueueSinglyNode<TYPE>();
                    allocator.deallocate(frontNode, 1);
                    node->nodeNext = nullptr;
                    frontNode = node;
                }
            }
        }

        void swap(SinglyListQueue<TYPE, ALLOCATOR>& other)
        {
            size_type tempSize = queueSize;
            node_pointer tempBack = backNode;
            node_pointer tempFront = frontNode;

            queueSize = other.queueSize;
            backNode = other.backNode;
            frontNode = other.frontNode;

            other.queueSize = tempSize;
            other.backNode = tempBack;
            other.frontNode = tempFront;
        }

        [[nodiscard]] bool operator == (const SinglyListQueue<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal{true};
            if (queueSize == other.queueSize)
            {
                node_pointer left = backNode;
                node_pointer right = other.backNode;

                while (left != nullptr && right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        equal = false;
                        break;
                    }
                    left = left->nodeNext;
                    right = right->nodeNext;
                }
            } else
            {
                equal = false;
            }
            return equal;
        }

        [[nodiscard]] bool operator != (const SinglyListQueue<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal{false};
            if (queueSize == other.queueSize)
            {
                node_pointer left = backNode;
                node_pointer right = other.backNode;

                while (left != nullptr && right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        equal = true;
                        break;
                    }
                    left = left->nodeNext;
                    right = right->nodeNext;
                }
            } else
            {
                equal = true;
            }
            return equal;
        }

        [[nodiscard]] bool operator >= (const SinglyListQueue<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal{};
            if (queueSize == other.queueSize)
            {
                equal = true;
                node_pointer left = backNode;
                node_pointer right = other.backNode;

                while (left != nullptr && right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value > right->value)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                    left = left->nodeNext;
                    right = right->nodeNext;
                }

            } else if (queueSize > other.queueSize)
            {
                equal = true;
                node_pointer left = backNode;
                node_pointer right = other.backNode;

                while (right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value > right->value)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                    left = left->nodeNext;
                    right = right->nodeNext;
                }

            } else
            {
                equal = false;
                node_pointer left = backNode;
                node_pointer right = other.backNode;

                while (left != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value > right->value)
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                    left = left->nodeNext;
                    right = right->nodeNext;
                }
            }
            return equal;
        }

        [[nodiscard]] bool operator <= (const SinglyListQueue<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;
            if (queueSize == other.queueSize)
            {
                equal = true;
                node_pointer left = backNode;
                node_pointer right = other.backNode;

                while (left != nullptr && right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value < right->value)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                    left = left->nodeNext;
                    right = right->nodeNext;
                }
            } else if (queueSize > other.queueSize)
            {
                equal = false;
                node_pointer left = backNode;
                node_pointer right = other.backNode;

                while (right != nullptr)
                {
                    if (left->value < right->value)
                    {
                        equal = true;
                        break;
                    } else
                    {
                        break;
                    }
                    left = left->nodeNext;
                    right = right->nodeNext;
                }
            } else
            {
                equal = true;
                node_pointer left = backNode;
                node_pointer right = other.backNode;
                while (left != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value < right->value)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                    left = left->nodeNext;
                    right = right->nodeNext;
                }
            }
            return equal;
        }

        [[nodiscard]] bool operator > (const SinglyListQueue<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal{};
            if (queueSize == other.queueSize)
            {
                equal = false;
                node_pointer left = backNode;
                node_pointer right = other.backNode;
                while (left != nullptr && right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value > right->value)
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                    left = left->nodeNext;
                    right = right->nodeNext;
                }
            } else if (queueSize > other.queueSize)
            {
                equal = true;
                node_pointer left = backNode;
                node_pointer right = other.backNode;
                while (right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value > right->value)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                    left = left->nodeNext;
                    right = right->nodeNext;
                }
            } else
            {
                equal = false;
                node_pointer left = backNode;
                node_pointer right = other.backNode;

                while (left != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value > right->value)
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                    left = left->nodeNext;
                    right = right->nodeNext;
                }
            }
            return equal;
        }

        [[nodiscard]] bool operator < (const SinglyListQueue<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal{};
            if (queueSize == other.queueSize)
            {
                equal = false;
                node_pointer left = backNode;
                node_pointer right = other.backNode;

                while (left != nullptr && right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value < right->value)
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
                node_pointer left = backNode;
                node_pointer right = other.backNode;

                while (right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value < right->value)
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                    left = left->nodeNext;
                    right = right->nodeNext;
                }
            } else
            {
                equal = true;
                node_pointer left = backNode;
                node_pointer right = other.backNode;

                while (left != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value < right->value)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                    left = left->nodeNext;
                    right = right->nodeNext;
                }
            }
            return equal;
        }

    private:

        real_allocator_type allocator;
        size_type queueSize;
        node_pointer backNode;
        node_pointer frontNode;
    };*/

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    class SinglyListQueue
    {
    public:

        using value_type                 = TYPE;
        using size_type                  = std::uint64_t;
        using difference_type            = std::int64_t;
        using allocator_type             = ALLOCATOR;
        using real_allocator_type        = typename std::allocator_traits<ALLOCATOR>::template rebind_alloc<QueueSinglyNode<TYPE>>;
        using reference                  = TYPE&;
        using const_reference            = const TYPE&;
        using pointer                    = TYPE*;
        using const_pointer              = const TYPE*;
        using node_pointer               = QueueSinglyNode<TYPE>*;
        using const_node_pointer         = const QueueSinglyNode<TYPE>*;

        SinglyListQueue(allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        frontNode{}, backNode{}
        {}

        explicit SinglyListQueue(const_reference value, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        frontNode{}, backNode{}
        {
            node_pointer workNode{nullptr};
            try
            {
                workNode = allocator.allocate(1);
                new(workNode) QueueSinglyNode<TYPE>{value_type(value), nullptr};
            } catch (std::bad_alloc)
            {
                workNode = nullptr;
                return;
            } catch (...)
            {
                (*workNode).~QueueSinglyNode<TYPE>();
                allocator.deallocate(workNode, 1);
                workNode = nullptr;
            }

            queueSize = 1;
            frontNode = workNode;
            backNode = workNode;
        }

        SinglyListQueue(std::initializer_list<value_type> list, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        frontNode{}, backNode{}
        {
            for(auto iter = list.begin(); iter != list.end(); ++iter)
            {
                push(*iter);
            }
        }

        template<typename ITERATOR>
        SinglyListQueue(ITERATOR begin, ITERATOR end, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        frontNode{}, backNode{}
        {
            difference_type distance{0};
            for(auto iter = begin; iter != end; ++iter)
            {
                ++distance;
            }
            if (distance > 0)
            {
                for(auto iter = begin; iter != end; ++iter)
                {
                    push(*iter);
                }
            }
        }

        SinglyListQueue(const SinglyListQueue<TYPE, ALLOCATOR>& other)
        {
            queueSize = other.queueSize;
            if (queueSize > 0)
            {
                node_pointer otherNode = other.frontNode;
                node_pointer workNode{nullptr};

                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) QueueSinglyNode<TYPE>{value_type(otherNode->value), nullptr};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    queueSize = 0;
                    frontNode = nullptr;
                    backNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueSinglyNode();
                    allocator.deallocate(workNode, 1);
                    queueSize = 0;
                    frontNode = nullptr;
                    backNode = nullptr;
                    workNode = nullptr;
                }
                frontNode = workNode;
                backNode = workNode;

                node_pointer previousNode = workNode;

                while (otherNode->next != nullptr)
                {
                    otherNode = otherNode->next;
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) QueueSinglyNode<TYPE>{value_type(otherNode->value), nullptr};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = frontNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~QueueSinglyNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        backNode = nullptr;
                        frontNode = nullptr;
                        queueSize = 0;
                        workNode = nullptr;
                        return;
                    } catch (...)
                    {
                        node_pointer currentNode = frontNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~QueueSinglyNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        backNode = nullptr;
                        frontNode = nullptr;
                        queueSize = 0;
                        workNode = nullptr;
                        return;
                    }
                    previousNode->next = workNode;
                    previousNode = previousNode->next;
                }
                backNode = workNode;
            } else
            {
                frontNode = nullptr;
                backNode = nullptr;
            }
        }

        SinglyListQueue<TYPE, ALLOCATOR>& operator = (const SinglyListQueue<TYPE, ALLOCATOR>& other)
        {
            if (queueSize == other.queueSize)
            {
                node_pointer currentNode = frontNode;
                node_pointer deleteNode = currentNode;

                while (currentNode != nullptr)
                {
                    currentNode = currentNode->next;
                    (*deleteNode).~QueueSinglyNode<TYPE>();
                    deleteNode = currentNode;
                }

                node_pointer otherNode = other.frontNode;
                node_pointer workNode = frontNode;

                while (otherNode != nullptr && workNode != nullptr)
                {
                    try
                    {
                        workNode->value = otherNode->value;
                    } catch (...)
                    {
                        node_pointer currentNode = frontNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~QueueSinglyNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        backNode = nullptr;
                        frontNode = nullptr;
                        queueSize = {};
                        return *this;
                    }
                    workNode = workNode->next;
                    otherNode = otherNode->next;
                }
            } else
            {
                node_pointer currentNode = frontNode;
                node_pointer deleteNode = currentNode;

                while (currentNode != nullptr)
                {
                    currentNode = currentNode->next;
                    (*deleteNode).~QueueSinglyNode<TYPE>();
                    allocator.deallocate(deleteNode, 1);
                    deleteNode = currentNode;
                }

                queueSize = other.queueSize;
                if (queueSize > 0)
                {
                    node_pointer otherNode = other.frontNode;
                    node_pointer workNode{nullptr};

                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) QueueSinglyNode<TYPE>{value_type(otherNode->value), nullptr};
                    } catch (std::bad_alloc)
                    {
                        workNode = nullptr;
                        queueSize = 0;
                        frontNode = nullptr;
                        backNode = nullptr;
                        return *this;
                    } catch (...)
                    {
                        (*workNode).~QueueSinglyNode();
                        allocator.deallocate(workNode, 1);
                        queueSize = 0;
                        frontNode = nullptr;
                        backNode = nullptr;
                        workNode = nullptr;
                    }
                    frontNode = workNode;
                    backNode = workNode;

                    node_pointer previousNode = workNode;

                    while (otherNode->next != nullptr)
                    {
                        otherNode = otherNode->next;
                        try
                        {
                            workNode = allocator.allocate(1);
                            new(workNode) QueueSinglyNode<TYPE>{value_type(otherNode->value), nullptr};
                        } catch (std::bad_alloc)
                        {
                            node_pointer currentNode = frontNode;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != nullptr)
                            {
                                currentNode = currentNode->next;
                                (*deleteNode).~QueueSinglyNode<TYPE>();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }
                            backNode = nullptr;
                            frontNode = nullptr;
                            queueSize = 0;
                            workNode = nullptr;
                            return *this;
                        } catch (...)
                        {
                            node_pointer currentNode = frontNode;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != nullptr)
                            {
                                currentNode = currentNode->next;
                                (*deleteNode).~QueueSinglyNode<TYPE>();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }
                            backNode = nullptr;
                            frontNode = nullptr;
                            queueSize = 0;
                            workNode = nullptr;
                            return *this;
                        }
                        previousNode->next = workNode;
                        previousNode = previousNode->next;
                    }
                    backNode = workNode;
                } else
                {
                    frontNode = nullptr;
                    backNode = nullptr;
                }
            }
            return *this;
        }

        SinglyListQueue(SinglyListQueue<TYPE, ALLOCATOR>&& other) noexcept :
        queueSize(std::move(other.queueSize)),
        frontNode(std::move(other.frontNode)),
        backNode(std::move(other.backNode))
        {
            other.queueSize = 0;
            other.frontNode = nullptr;
            other.backNode = nullptr;
        }

        SinglyListQueue<TYPE, ALLOCATOR>& operator = (SinglyListQueue<TYPE, ALLOCATOR>&& other) noexcept
        {
            node_pointer currentNode = frontNode;
            node_pointer deleteNode = currentNode;

            while (currentNode != nullptr)
            {
                currentNode = currentNode->next;
                (*deleteNode).~QueueSinglyNode<TYPE>();
                allocator.deallocate(deleteNode, 1);
                deleteNode = currentNode;
            }

            queueSize = std::move(other.queueSize);
            frontNode = std::move(other.frontNode);
            backNode = std::move(other.backNode);

            other.backNode = nullptr;
            other.frontNode = nullptr;
            other.queueSize = 0;

            return *this;
        }

        virtual ~SinglyListQueue()
        {
            node_pointer currentNode = frontNode;
            node_pointer deleteNode = currentNode;

            while (currentNode != nullptr)
            {
                currentNode = currentNode->next;
                (*deleteNode).~QueueSinglyNode<TYPE>();
                allocator.deallocate(deleteNode, 1);
                deleteNode = currentNode;
            }
            backNode = nullptr;
            frontNode = nullptr;
            queueSize = {};
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
                return frontNode->value;
            } else
            {
                throw std::out_of_range("Queue is empty");
            }
        }

        [[nodiscard]] inline const_reference front() const
        {
            if (queueSize > 0)
            {
                return frontNode->value;
            } else
            {
                throw std::out_of_range("Queue is empty");
            }
        }

        [[nodiscard]] inline reference back()
        {
            if (queueSize > 0)
            {
                return backNode->value;
            } else
            {
                throw std::out_of_range("Queue is empty");
            }
        }

        [[nodiscard]] inline const_reference back() const
        {
            if (queueSize > 0)
            {
                return backNode->value;
            } else
            {
                throw std::out_of_range("Queue is empty");
            }
        }

        void push(const_reference value)
        {
            if (queueSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) QueueSinglyNode<TYPE>{value_type(value), nullptr};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueSinglyNode();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    return;
                }
                queueSize = 1;
                frontNode = workNode;
                backNode = workNode;
            } else if (queueSize == 1)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) QueueSinglyNode<TYPE>{value_type(value), nullptr};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueSinglyNode();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    return;
                }
                frontNode->next = workNode;
                backNode = workNode;
                queueSize = 2;
            } else
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) QueueSinglyNode<TYPE>{value_type(value), nullptr};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueSinglyNode();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    return;
                }

                backNode->next = workNode;
                backNode = workNode;
                ++queueSize;
            }
        }

        void push(value_type&& value)
        {
            if (queueSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                }
                new(workNode) QueueSinglyNode<TYPE>{value_type(std::move(value)), nullptr};
                queueSize = 1;
                frontNode = workNode;
                backNode = workNode;
            } else if (queueSize == 1)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);

                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                }

                new(workNode) QueueSinglyNode<TYPE>{value_type(std::move(value)), nullptr};
                frontNode->next = workNode;
                backNode = workNode;
                queueSize = 2;
            } else
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                }

                new(workNode) QueueSinglyNode<TYPE>{value_type(std::move(value)), nullptr};
                backNode->next = workNode;
                backNode = workNode;
                ++queueSize;
            }
        }

        void pop()
        {
            if (queueSize == 1)
            {
                (*frontNode).~QueueSinglyNode<TYPE>();
                allocator.deallocate(frontNode, 1);
                queueSize = 0;
                frontNode = nullptr;
                backNode = nullptr;
            } else
            {
                node_pointer next = frontNode->next;
                (*frontNode).~QueueSinglyNode<TYPE>();
                allocator.deallocate(frontNode, 1);
                frontNode = next;
                --queueSize;
            }
        }

        template<typename... Args>
        void emplace(Args... args)
        {
            if (queueSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) QueueSinglyNode<TYPE>{value_type(std::forward<Args>(args)...), nullptr};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueSinglyNode();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    return;
                }
                queueSize = 1;
                frontNode = workNode;
                backNode = workNode;
            } else if (queueSize == 1)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) QueueSinglyNode<TYPE>{value_type(std::forward<Args>(args)...), nullptr};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueSinglyNode();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    return;
                }
                frontNode->next = workNode;
                backNode = workNode;
                queueSize = 2;
            } else
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) QueueSinglyNode<TYPE>{value_type(std::forward<Args>(args)...), nullptr};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueSinglyNode();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    return;
                }

                backNode->next = workNode;
                backNode = workNode;
                ++queueSize;
            }
        }

        void swap(SinglyListQueue<TYPE, ALLOCATOR>& other)
        {
            auto tempSize = other.queueSize;
            auto tempFront = other.frontNode;
            auto tempBack = other.backNode;

            other.queueSize = queueSize;
            other.frontNode = frontNode;
            other.backNode = backNode;

            queueSize = tempSize;
            frontNode = tempFront;
            backNode = tempBack;
        }

        [[nodiscard]] bool operator == (const SinglyListQueue<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal{true};
            if (queueSize == other.queueSize)
            {
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr && right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        equal = false;
                        break;
                    }
                    left = left->next;
                    right = right->next;
                }
            } else
            {
                equal = false;
            }
            return equal;
        }

        [[nodiscard]] bool operator != (const SinglyListQueue<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal{false};
            if (queueSize == other.queueSize)
            {
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr && right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        equal = true;
                        break;
                    }
                    left = left->next;
                    right = right->next;
                }
            } else
            {
                equal = true;
            }
            return equal;
        }

        [[nodiscard]] bool operator >= (const SinglyListQueue<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal{};
            if (queueSize == other.queueSize)
            {
                equal = true;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr && right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value > right->value)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                    left = left->next;
                    right = right->next;
                }

            } else if (queueSize > other.queueSize)
            {
                equal = true;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value > right->value)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                    left = left->next;
                    right = right->next;
                }

            } else
            {
                equal = false;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value > right->value)
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                    left = left->next;
                    right = right->next;
                }
            }
            return equal;
        }

        [[nodiscard]] bool operator <= (const SinglyListQueue<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;
            if (queueSize == other.queueSize)
            {
                equal = true;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr && right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value < right->value)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                    left = left->next;
                    right = right->next;
                }
            } else if (queueSize > other.queueSize)
            {
                equal = false;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (right != nullptr)
                {
                    if (left->value < right->value)
                    {
                        equal = true;
                        break;
                    } else
                    {
                        break;
                    }
                    left = left->next;
                    right = right->next;
                }
            } else
            {
                equal = true;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;
                while (left != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value < right->value)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                    left = left->next;
                    right = right->next;
                }
            }
            return equal;
        }

        [[nodiscard]] bool operator > (const SinglyListQueue<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal{};
            if (queueSize == other.queueSize)
            {
                equal = false;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;
                while (left != nullptr && right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value > right->value)
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                    left = left->next;
                    right = right->next;
                }
            } else if (queueSize > other.queueSize)
            {
                equal = true;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;
                while (right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value > right->value)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                    left = left->next;
                    right = right->next;
                }
            } else
            {
                equal = false;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value > right->value)
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                    left = left->next;
                    right = right->next;
                }
            }
            return equal;
        }

        [[nodiscard]] bool operator < (const SinglyListQueue<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal{};
            if (queueSize == other.queueSize)
            {
                equal = false;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr && right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value < right->value)
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
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (right != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value < right->value)
                        {
                            equal = true;
                            break;
                        } else
                        {
                            break;
                        }
                    }
                    left = left->next;
                    right = right->next;
                }
            } else
            {
                equal = true;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr)
                {
                    if (left->value != right->value)
                    {
                        if (left->value < right->value)
                        {
                            break;
                        } else
                        {
                            equal = false;
                            break;
                        }
                    }
                    left = left->next;
                    right = right->next;
                }
            }
            return equal;
        }

    private:

        real_allocator_type allocator;
        size_type queueSize;
        node_pointer frontNode;
        node_pointer backNode;
    };



}

#endif //THREADPOOL_SINGLY_LIST_QUEUE_H
