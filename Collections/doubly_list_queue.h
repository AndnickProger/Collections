#ifndef THREADPOOL_DOUBLY_LIST_QUEUE_H
#define THREADPOOL_DOUBLY_LIST_QUEUE_H

#include <stdexcept>
#include <stdint.h>
#include <stdlib.h>
#include <ostream>
#include <iostream>

namespace th_pool
{
    template<typename TYPE>
    struct QueueDoublyNode
    {
        using value_type                 = TYPE;
        using size_type                  = std::uint64_t;
        using difference_type            = std::int64_t;
        using reference                  = TYPE&;
        using const_reference            = const TYPE&;
        using pointer                    = TYPE*;
        using const_pointer              = const TYPE*;
        using node_pointer               = QueueDoublyNode<TYPE>*;
        using const_node_pointer         = const QueueDoublyNode<TYPE>*;

        [[nodiscard]] inline bool operator == (const QueueDoublyNode<TYPE>& other)
        {
            return value == other.value && previous == other.previous && next == other.next;
        }

        [[nodiscard]] inline bool operator != (const QueueDoublyNode<TYPE>& other)
        {
            return value != other.value || previous != other.previous || next != other.next;
        }

        value_type value;
        node_pointer previous;
        node_pointer next;
    };

    template<typename TYPE>
    std::ostream& operator << (std::ostream& os, const QueueDoublyNode<TYPE>& node)
    {
        os << node.value;
        return os;
    }

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    class DoublyListQueue
    {
    public:

        using value_type                 = TYPE;
        using size_type                  = std::uint64_t;
        using difference_type            = std::int64_t;
        using allocator_type             = ALLOCATOR;
        using real_allocator_type        = typename std::allocator_traits<ALLOCATOR>::template rebind_alloc<QueueDoublyNode<TYPE>>;
        using reference                  = TYPE&;
        using const_reference            = const TYPE&;
        using pointer                    = TYPE*;
        using const_pointer              = const TYPE*;
        using node_pointer               = QueueDoublyNode<TYPE>*;
        using const_node_pointer         = const QueueDoublyNode<TYPE>*;

        DoublyListQueue(ALLOCATOR allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        backNode{}, frontNode{}
        {}

        explicit DoublyListQueue(const_reference value, ALLOCATOR allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        backNode{}, frontNode{}
        {
            node_pointer workNode{};
            try
            {
                workNode = allocator.allocate(1);
                new(workNode) QueueDoublyNode<TYPE>{value, nullptr};
            } catch (std::bad_alloc)
            {
                workNode = nullptr;
                return;
            }catch (...)
            {
                (*workNode).~QueueDoublyNode<TYPE>();
                allocator.deallocate(workNode, 1);
                return;
            }
            workNode->next = nullptr;
            workNode->previous = nullptr;

            backNode = workNode;
            frontNode = backNode;
            queueSize = 1;

        }

        DoublyListQueue(std::initializer_list<value_type> list, ALLOCATOR allocator_ = ALLOCATOR()) :
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
        DoublyListQueue(ITERATOR begin, ITERATOR end, ALLOCATOR allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        queueSize{},
        backNode{},frontNode{}
        {
            difference_type distance = {};
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

        DoublyListQueue(const DoublyListQueue<TYPE>& other)
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
                    new(workNode) QueueDoublyNode<TYPE>{value_type(otherNode->value), nullptr, nullptr};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    queueSize = 0;
                    backNode = nullptr;
                    frontNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueDoublyNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    queueSize = 0;
                    backNode = nullptr;
                    frontNode = nullptr;
                    return;
                }
                backNode = workNode;

                while (otherNode->next != nullptr)
                {
                    otherNode = otherNode->next;
                    previousNode = workNode;
                    try
                    {
                        workNode = allocator.allocate(1);
                        //workNode->value = otherNode->value;
                        new(workNode) QueueDoublyNode<TYPE>{value_type(otherNode->value), previousNode, nullptr};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = backNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            currentNode = currentNode->next;
                            (*workNode).~QueueDoublyNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        backNode = nullptr;
                        frontNode = nullptr;
                        queueSize = 0;
                        return;
                    }catch (...)
                    {
                        allocator.deallocate(workNode, 1);
                        node_pointer currentNode = backNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            currentNode = currentNode->next;
                            (*workNode).~QueueDoublyNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        backNode = nullptr;
                        frontNode = nullptr;
                        queueSize = 0;
                        return;

                    }

                    workNode->next = nullptr;
                }
                frontNode = workNode;

            } else
            {
                backNode = nullptr;
                frontNode = nullptr;
            }
        }

        DoublyListQueue<TYPE>& operator = (const DoublyListQueue<TYPE>& other)
        {
            if (queueSize == other.queueSize)
            {
                node_pointer currentNode = backNode;
                node_pointer deleteNode = currentNode;

                while (currentNode != nullptr)
                {
                    currentNode = currentNode->next;
                    (*deleteNode).~QueueDoublyNode<TYPE>();
                    deleteNode = currentNode;
                }

                currentNode = backNode;
                node_pointer otherNode = other.backNode;

                try
                {
                    while (currentNode != nullptr && otherNode != nullptr)
                    {
                        currentNode->value = otherNode->value;
                        currentNode = currentNode->next;
                        otherNode = otherNode->next;
                    }
                } catch (...)
                {
                    node_pointer currentNode = backNode;
                    node_pointer deleteNode = currentNode;

                    while (currentNode != nullptr)
                    {
                        currentNode = currentNode->next;
                        (*deleteNode).~QueueDoublyNode<TYPE>();
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
                    currentNode = currentNode->next;
                    (*deleteNode).~QueueDoublyNode<TYPE>();
                    allocator.deallocate(deleteNode, 1);
                    deleteNode = currentNode;
                }
                backNode = nullptr;
                frontNode = nullptr;
                queueSize = {};

                queueSize = other.queueSize;
                if (queueSize > 0)
                {
                    node_pointer otherNode = other.backNode;
                    node_pointer previousNode;
                    node_pointer workNode;
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) QueueDoublyNode<TYPE>{value_type(otherNode->value), nullptr, nullptr};
                    } catch (std::bad_alloc)
                    {
                        workNode = nullptr;
                        queueSize = 0;
                        backNode = nullptr;
                        frontNode = nullptr;
                        return *this;
                    } catch (...)
                    {
                        (*workNode).~QueueDoublyNode<TYPE>();
                        allocator.deallocate(workNode, 1);
                        queueSize = 0;
                        backNode = nullptr;
                        frontNode = nullptr;
                        return *this;
                    }
                    backNode = workNode;

                    while (otherNode->next != nullptr)
                    {
                        otherNode = otherNode->next;
                        previousNode = workNode;
                        try
                        {
                            workNode = allocator.allocate(1);
                            new(workNode) QueueDoublyNode<TYPE>{value_type(otherNode->value), previousNode, nullptr};
                        } catch (std::bad_alloc)
                        {
                            node_pointer currentNode = backNode;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != nullptr)
                            {
                                currentNode = currentNode->next;
                                (*workNode).~QueueDoublyNode<TYPE>();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }
                            backNode = nullptr;
                            frontNode = nullptr;
                            queueSize = 0;
                            return *this;
                        } catch (...)
                        {
                            (*workNode).~QueueDoublyNode<TYPE>();
                            allocator.deallocate(workNode, 1);
                            node_pointer currentNode = backNode;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != nullptr)
                            {
                                currentNode = currentNode->next;
                                (*workNode).~QueueDoublyNode<TYPE>();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }
                            backNode = nullptr;
                            frontNode = nullptr;
                            queueSize = 0;
                            return *this;

                        }
                        previousNode->next = workNode;
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

        DoublyListQueue(DoublyListQueue<TYPE>&& other) noexcept :
        queueSize(std::move(other.queueSize)),
        backNode(std::move(other.backNode)),
        frontNode(std::move(other.frontNode))
        {
            other.queueSize = {};
            other.backNode = {};
            other.frontNode = {};
        }

        DoublyListQueue<TYPE>& operator = (DoublyListQueue<TYPE>&& other) noexcept
        {
            node_pointer currentNode = backNode;
            node_pointer deleteNode = currentNode;

            while (currentNode != nullptr)
            {
                currentNode = currentNode->next;
                (*deleteNode).~QueueDoublyNode<TYPE>();
                allocator.deallocate(deleteNode, 1);
                deleteNode = currentNode;
            }

            queueSize = std::move(other.queueSize);
            backNode = std::move(other.backNode);
            frontNode = std::move(other.frontNode);

            other.queueSize = {};
            other.backNode = nullptr;
            other.frontNode = nullptr;

            return *this;
        }

        virtual ~DoublyListQueue()
        {
            node_pointer currentNode = backNode;
            node_pointer deleteNode = currentNode;

            while (currentNode != nullptr)
            {
                currentNode = currentNode->next;
                (*deleteNode).~QueueDoublyNode<TYPE>();
                allocator.deallocate(deleteNode, 1);
                deleteNode = currentNode;
            }
            backNode = nullptr;
            frontNode = nullptr;
            queueSize = {};
        }

        [[nodiscard]] inline size_type size() const noexcept
        {
            return queueSize;
        }

        [[nodiscard]] inline bool empty() const noexcept
        {
            return queueSize == 0;
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
                    new(backNode) QueueDoublyNode<TYPE>{value_type(value), nullptr, nullptr};
                } catch (std::bad_alloc)
                {
                    queueSize = 0;
                    backNode = nullptr;
                    frontNode = nullptr;
                    return;
                } catch (...)
                {
                    (*backNode).~QueueDoublyNode<TYPE>();
                    allocator.deallocate(backNode, 1);
                    queueSize = 0;
                    backNode = nullptr;
                    frontNode = nullptr;
                    return;
                }
                frontNode = backNode;
                queueSize = 1;
            } else if (queueSize == 1)
            {
                node_pointer workNode{};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) QueueDoublyNode<TYPE>{value_type(value), nullptr, frontNode};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueDoublyNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                queueSize = 2;
                backNode = workNode;
                frontNode->previous = backNode;
                frontNode->next = nullptr;
            } else
            {
                node_pointer node{};
                try
                {
                    node = allocator.allocate(1);
                    new(node) QueueDoublyNode<TYPE>{value_type(value), nullptr, backNode};
                } catch (std::bad_alloc)
                {
                    node = nullptr;
                    return;
                } catch (...)
                {
                    (*backNode).~QueueDoublyNode<TYPE>();
                    allocator.deallocate(node, 1);
                    return;
                }
                backNode->previous = node;
                backNode = node;
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
                new(backNode) QueueDoublyNode<TYPE>{std::move(value), nullptr, nullptr};
                queueSize = 1;
                frontNode = backNode;
            } else if (queueSize == 1)
            {
                node_pointer node{};
                try
                {
                    node = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    node = nullptr;
                    return;
                }
                new(node) QueueDoublyNode<TYPE>{std::move(value), nullptr, frontNode};
                queueSize = 2;
                backNode = node;
                frontNode->previous = backNode;
                frontNode->next = nullptr;
            } else
            {
                node_pointer node{};
                try
                {
                    node = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    node = nullptr;
                    return;
                }
                new(node) QueueDoublyNode<TYPE>{std::move(value), nullptr, backNode};
                backNode->previous = node;
                backNode = node;
                ++queueSize;
            }
        }

        void pop()
        {

            if (queueSize == 1)
            {
                queueSize = 0;
                (*backNode).~QueueDoublyNode<TYPE>();
                allocator.deallocate(backNode, 1);
                backNode = nullptr;
                frontNode = nullptr;
            } else if (queueSize == 2)
            {
                queueSize = 1;
                (*frontNode).~QueueDoublyNode<TYPE>();
                allocator.deallocate(frontNode, 1);
                backNode->next = nullptr;
                frontNode = backNode;
            } else if (queueSize > 2)
            {
                --queueSize;
                node_pointer previous = frontNode->previous;
                (*frontNode).~QueueDoublyNode<TYPE>();
                allocator.deallocate(frontNode, 1);
                previous->next = nullptr;
                frontNode = previous;
            }
        }

        void swap(DoublyListQueue<TYPE, ALLOCATOR>& other)
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

        template<typename... Args>
        void emplace(Args... args)
        {
            if (queueSize == 0)
            {
                node_pointer workNode = nullptr;
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) QueueDoublyNode<TYPE>{TYPE(std::forward<Args>(args)...), nullptr, nullptr};

                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    queueSize = 0;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueDoublyNode<TYPE>();
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
                    new(workNode) QueueDoublyNode<TYPE>{TYPE(std::forward<Args>(args)...), nullptr, frontNode};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    queueSize = 1;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueDoublyNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    queueSize = 1;
                    return;
                }
                frontNode->previous = workNode;
                frontNode->next = nullptr;
                backNode = workNode;
                queueSize = 2;
            } else
            {
                node_pointer workNode = nullptr;
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) QueueDoublyNode<TYPE>{TYPE(std::forward<Args>(args)...), nullptr, backNode};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~QueueDoublyNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    return;
                }
                backNode->previous = workNode;
                backNode = workNode;
                ++queueSize;
            }
        }

        [[nodiscard]] bool operator == (const DoublyListQueue<TYPE, ALLOCATOR>& other)
        {
            bool equal = true;
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
                    left = left->next;
                    right = right->next;
                }
            } else
            {
                equal = false;
            }
            return equal;
        }

        [[nodiscard]] bool operator != (const DoublyListQueue<TYPE, ALLOCATOR>& other)
        {
            bool equal = false;
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
                    left = left->next;
                    right = right->next;
                }
            } else
            {
                equal = true;
            }
            return equal;
        }

        [[nodiscard]] bool operator >= (const DoublyListQueue<TYPE, ALLOCATOR>& other)
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
                    left = left->next;
                    right = right->next;
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
                    left = left->next;
                    right = right->next;
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
                    left = left->next;
                    right = right->next;
                }
            }
            return equal;
        }

        [[nodiscard]] bool operator > (const DoublyListQueue<TYPE, ALLOCATOR>& other)
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
                    left = left->next;
                    right = right->next;
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
                    left = left->next;
                    right = right->next;
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
                    left = left->next;
                    right = right->next;
                }
            }
            return equal;
        }

        [[nodiscard]] bool operator <= (const DoublyListQueue<TYPE, ALLOCATOR>& other)
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
                    left = left->next;
                    right = right->next;
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
                    left = left->next;
                    right = right->next;
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
                    left = left->next;
                    right = right->next;
                }
            }
            return equal;
        }

        [[nodiscard]] bool operator < (const DoublyListQueue<TYPE, ALLOCATOR>& other)
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
                    left = left->next;
                    right = right->next;
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
                    left = left->next;
                    right = right->next;
                }
            }
            return equal;
        }

    private:

        real_allocator_type allocator;
        size_type queueSize;
        node_pointer backNode;
        node_pointer frontNode;
    };
}

#endif //THREADPOOL_DOUBLY_LIST_QUEUE_H
