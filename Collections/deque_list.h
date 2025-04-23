#ifndef INC_3DGAME_DEQUE_LIST_H
#define INC_3DGAME_DEQUE_LIST_H

#include <stdexcept>
#include <cstdlib>
#include <cstdint>
#include <memory>

namespace bice
{
    template<typename TYPE>
    struct DequeNode
    {
        using value_type                       = TYPE;
        using reference                        = TYPE&;
        using const_reference                  = const TYPE&;
        using pointer                          = TYPE*;
        using const_pointer                    = const TYPE*;
        using node_pointer                     = DequeNode<TYPE>*;
        using const_node_pointer               = const DequeNode<TYPE>*;

        [[nodiscard]] inline bool operator == (const DequeNode<TYPE>& other) const noexcept
        {
            return data == other.data && previous == other.previous && next == other.next;
        }

        [[nodiscard]] inline bool operator != (const DequeNode<TYPE>& other) const noexcept
        {
            return data != other.data || previous != other.previous || next != other.next;
        }

        value_type data;
        node_pointer previous;
        node_pointer next;
    };

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    class DequeList
    {
    public:

        using value_type                   = TYPE;
        using size_type                    = std::uint64_t;
        using difference_type              = std::int64_t;
        using allocator_type               = ALLOCATOR;
        using real_allocator_type          = typename std::allocator_traits<ALLOCATOR>::template rebind_alloc<DequeNode<TYPE>>;
        using reference                    = TYPE&;
        using const_reference              = const TYPE&;
        using pointer                      = TYPE*;
        using const_pointer                = const TYPE*;
        using node_pointer                 = DequeNode<TYPE>*;
        using const_node_pointer           = const DequeNode<TYPE>*;

        DequeList(allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        dequeSize{},
        frontNode{}, backNode{}
        {}

        explicit DequeList(const size_type size, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        dequeSize{},
        frontNode{}, backNode{}
        {
            if (size > 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) DequeNode<TYPE>{value_type(), nullptr, nullptr};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~DequeNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    return;
                }

                frontNode = workNode;
                backNode = workNode;
                dequeSize = 1;

                node_pointer previousNode = frontNode;

                for(size_type idx = 0; idx < size - 1; ++idx)
                {
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) DequeNode<TYPE>{value_type(), previousNode, nullptr};
                        //previousNode->node = workNode;
                        //previousNode = workNode;
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = frontNode;
                        node_pointer deleteNode = currentNode;
                        while (currentNode != nullptr)
                        {
                            (*deleteNode).~DequeNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            currentNode = currentNode->next;
                            deleteNode = currentNode;
                        }
                        dequeSize = 0;
                        frontNode = nullptr;
                        backNode = nullptr;
                        return;
                    } catch (...)
                    {
                        node_pointer currentNode = frontNode;
                        node_pointer deleteNode = currentNode;
                        while (currentNode != nullptr)
                        {
                            (*deleteNode).~DequeNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            currentNode = currentNode->next;
                            deleteNode = currentNode;
                        }
                        (*workNode).~DequeNode<TYPE>();
                        allocator.deallocate(workNode, 1);
                        dequeSize = 0;
                        frontNode = nullptr;
                        backNode = nullptr;
                        return;
                    }

                    previousNode->next = workNode;
                    previousNode = workNode;
                }
                backNode = workNode;
                dequeSize = size;
            }
        }

        DequeList(const size_type size, const_reference value, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        dequeSize{},
        frontNode{}, backNode{}
        {
            if (size > 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) DequeNode<TYPE>{value_type(value), nullptr, nullptr};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~DequeNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    return;
                }

                frontNode = workNode;
                backNode = workNode;
                dequeSize = 1;

                node_pointer previousNode = frontNode;

                for(size_type idx = 0; idx < size - 1; ++idx)
                {
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) DequeNode<TYPE>{value_type(value), previousNode, nullptr};
                        //previousNode->node = workNode;
                        //previousNode = workNode;
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = frontNode;
                        node_pointer deleteNode = currentNode;
                        while (currentNode != nullptr)
                        {
                            (*deleteNode).~DequeNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            currentNode = currentNode->next;
                            deleteNode = currentNode;
                        }
                        dequeSize = 0;
                        frontNode = nullptr;
                        backNode = nullptr;
                        return;
                    } catch (...)
                    {
                        node_pointer currentNode = frontNode;
                        node_pointer deleteNode = currentNode;
                        while (currentNode != nullptr)
                        {
                            (*deleteNode).~DequeNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            currentNode = currentNode->next;
                            deleteNode = currentNode;
                        }
                        (*workNode).~DequeNode<TYPE>();
                        allocator.deallocate(workNode, 1);
                        dequeSize = 0;
                        frontNode = nullptr;
                        backNode = nullptr;
                        return;
                    }

                    previousNode->next = workNode;
                    previousNode = workNode;
                }
                backNode = workNode;
                dequeSize = size;
            }
        }

        DequeList(std::initializer_list<value_type> list, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        dequeSize{},
        frontNode{}, backNode{}
        {
            if (list.size() > 0)
            {
                node_pointer workNode{nullptr};
                node_pointer previousNode{nullptr};
                auto iter = list.begin();
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) DequeNode<value_type>{value_type(*iter), nullptr, nullptr};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~DequeNode<value_type>();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                ++iter;

                frontNode = workNode;
                backNode = workNode;

                while (iter != list.end())
                {
                    previousNode = workNode;
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) DequeNode<value_type>{value_type(*iter), previousNode, nullptr};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = frontNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            (*deleteNode).~DequeNode<value_type>();
                            allocator.deallocate(deleteNode, 1);
                            currentNode = currentNode->next;
                            deleteNode = currentNode;
                        }
                        dequeSize = 0;
                        frontNode = nullptr;
                        backNode = nullptr;
                        return;
                    } catch (...)
                    {
                        node_pointer currentNode = frontNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            (*deleteNode).~DequeNode<value_type>();
                            allocator.deallocate(deleteNode, 1);
                            currentNode = currentNode->next;
                            deleteNode = currentNode;
                        }
                        (*workNode).~DequeNode<value_type>();
                        allocator.deallocate(workNode, 1);
                        dequeSize = 0;
                        frontNode = nullptr;
                        backNode = nullptr;
                        return;
                    }
                    ++iter;
                    previousNode->next = workNode;
                }
                backNode = workNode;
                dequeSize = list.size();
            }
        }

        template<typename ITERATOR>
        DequeList(const ITERATOR begin, const ITERATOR end, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        dequeSize{},
        frontNode{}, backNode{}
        {
            difference_type distance{0};
            for(auto iter = begin; iter != end; ++iter)
            {
                ++distance;
            }
            //auto iter = begin;
            if (distance > 0)
            {
                node_pointer workNode{nullptr};
                node_pointer previousNode{nullptr};
                auto iter = begin;
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) DequeNode<value_type>{*iter, nullptr, nullptr};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~DequeNode<value_type>();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                ++iter;

                frontNode = workNode;
                backNode = workNode;

                while (iter != end)
                {
                    previousNode = workNode;
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) DequeNode<value_type>{value_type(*iter), previousNode, nullptr};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = frontNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            (*deleteNode).~DequeNode<value_type>();
                            allocator.deallocate(deleteNode, 1);
                            currentNode = currentNode->next;
                            deleteNode = currentNode;
                        }
                        dequeSize = 0;
                        frontNode = nullptr;
                        backNode = nullptr;
                        return;
                    } catch (...)
                    {
                        node_pointer currentNode = frontNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            (*deleteNode).~DequeNode<value_type>();
                            allocator.deallocate(deleteNode, 1);
                            currentNode = currentNode->next;
                            deleteNode = currentNode;
                        }
                        (*workNode).~DequeNode<value_type>();
                        allocator.deallocate(workNode, 1);
                        dequeSize = 0;
                        frontNode = nullptr;
                        backNode = nullptr;
                        return;
                    }

                    ++iter;
                    previousNode->next = workNode;
                }
                backNode = workNode;
                dequeSize = distance;
            }
        }

        DequeList(const DequeList<TYPE, ALLOCATOR>& other)
        {
            dequeSize = other.dequeSize;
            frontNode = nullptr;
            backNode = nullptr;
            if (dequeSize > 0)
            {
                node_pointer otherNode = other.frontNode;
                node_pointer workNode{nullptr};

                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) DequeNode<value_type>{value_type(otherNode->data), nullptr, nullptr};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~DequeNode<value_type>();
                    allocator.deallocate(workNode, 1);
                    return;
                }

                frontNode = workNode;
                backNode = workNode;
                otherNode = otherNode->next;

                node_pointer previousNode{nullptr};

                while (otherNode != nullptr)
                {
                    previousNode = workNode;
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) DequeNode<value_type>{value_type(otherNode->data), previousNode, nullptr};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = frontNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            (*deleteNode).~DequeNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            currentNode = currentNode->next;
                            deleteNode = currentNode;
                        }

                        dequeSize = 0;
                        frontNode = nullptr;
                        backNode = nullptr;
                        return;
                    } catch (...)
                    {
                        node_pointer currentNode = frontNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            (*deleteNode).~DequeNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            currentNode = currentNode->next;
                            deleteNode = currentNode;
                        }

                        (*workNode).~DequeNode<TYPE>();
                        allocator.deallocate(workNode, 1);
                        dequeSize = 0;
                        frontNode = nullptr;
                        backNode = nullptr;
                        return;
                    }
                    otherNode = otherNode->next;
                    previousNode->next = workNode;
                    previousNode = workNode;
                }
                backNode = workNode;
            }
        }

        DequeList<TYPE, ALLOCATOR>& operator = (const DequeList<TYPE, ALLOCATOR>& other)
        {
            if (dequeSize == other.dequeSize && dequeSize > 0)
            {
                node_pointer currentNode = frontNode;
                node_pointer otherNode = other.frontNode;

                while (otherNode != nullptr)
                {
                    try
                    {
                        currentNode->data = otherNode->data;
                    } catch (...)
                    {
                        node_pointer currentNode = frontNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            (*deleteNode).~DequeNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            currentNode = currentNode->next;
                            deleteNode = currentNode;
                        }
                        dequeSize = 0;
                        frontNode = nullptr;
                        backNode = nullptr;
                        return *this;
                    }
                    currentNode = currentNode->next;
                    otherNode = otherNode->next;
                }
            } else
            {
                node_pointer currentNode = frontNode;
                node_pointer deleteNode = currentNode;

                while (currentNode != nullptr)
                {
                    (*deleteNode).~DequeNode<TYPE>();
                    allocator.deallocate(deleteNode, 1);
                    currentNode = currentNode->next;
                    deleteNode = currentNode;
                }

                dequeSize = other.dequeSize;
                frontNode = nullptr;
                backNode = nullptr;
                if (dequeSize > 0)
                {
                    node_pointer otherNode = other.frontNode;
                    node_pointer workNode{nullptr};

                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) DequeNode<value_type>{value_type(otherNode->data), nullptr, nullptr};
                    } catch (std::bad_alloc)
                    {
                        return *this;
                    } catch (...)
                    {
                        (*workNode).~DequeNode<value_type>();
                        allocator.deallocate(workNode, 1);
                        return *this;
                    }

                    frontNode = workNode;
                    backNode = workNode;
                    otherNode = otherNode->next;

                    node_pointer previousNode{nullptr};

                    while (otherNode != nullptr)
                    {
                        previousNode = workNode;
                        try
                        {
                            workNode = allocator.allocate(1);
                            new(workNode) DequeNode<value_type>{value_type(otherNode->data), previousNode, nullptr};
                        } catch (std::bad_alloc)
                        {
                            node_pointer currentNode = frontNode;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != nullptr)
                            {
                                (*deleteNode).~DequeNode<TYPE>();
                                allocator.deallocate(deleteNode, 1);
                                currentNode = currentNode->next;
                                deleteNode = currentNode;
                            }

                            dequeSize = 0;
                            frontNode = nullptr;
                            backNode = nullptr;
                            return *this;
                        } catch (...)
                        {
                            node_pointer currentNode = frontNode;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != nullptr)
                            {
                                (*deleteNode).~DequeNode<TYPE>();
                                allocator.deallocate(deleteNode, 1);
                                currentNode = currentNode->next;
                                deleteNode = currentNode;
                            }

                            (*workNode).~DequeNode<TYPE>();
                            allocator.deallocate(workNode, 1);
                            dequeSize = 0;
                            frontNode = nullptr;
                            backNode = nullptr;
                            return *this;
                        }
                        otherNode = otherNode->next;
                        previousNode->next = workNode;
                        previousNode = workNode;
                    }
                    backNode = workNode;
                }
            }

            return *this;
        }

        DequeList(DequeList<TYPE, ALLOCATOR>&& other) noexcept :
        dequeSize(std::move(other.dequeSize)),
        frontNode(std::move(other.frontNode)),
        backNode(std::move(other.backNode))
        {
            other.frontNode = nullptr;
            other.backNode = nullptr;
        }

        DequeList<TYPE, ALLOCATOR>& operator = (DequeList<TYPE, ALLOCATOR>&& other) noexcept
        {
            if (dequeSize > 0 && frontNode != nullptr)
            {
                node_pointer currentNode = frontNode;
                node_pointer deleteNode = currentNode;

                while (currentNode != nullptr)
                {
                    (*deleteNode).~DequeNode<TYPE>();
                    allocator.deallocate(deleteNode, 1);
                    currentNode = currentNode->next;
                    deleteNode = currentNode;
                }
            }

            dequeSize = std::move(other.dequeSize);
            frontNode = std::move(other.frontNode);
            backNode = std::move(other.backNode);

            other.frontNode = nullptr;
            other.backNode = nullptr;

            return *this;
        }

        virtual ~DequeList()
        {
            if (dequeSize > 0 && frontNode != nullptr)
            {
                node_pointer currentNode = frontNode;
                node_pointer deleteNode = currentNode;

                while (currentNode != nullptr)
                {
                    (*deleteNode).~DequeNode<TYPE>();
                    allocator.deallocate(deleteNode, 1);
                    currentNode = currentNode->next;
                    deleteNode = currentNode;
                }
            }
        }

        [[nodiscard]] inline size_type size() const noexcept
        {
            return dequeSize;
        }

        [[nodiscard]] inline bool empty() const noexcept
        {
            return dequeSize == 0;
        }

        [[nodiscard]] inline reference front()
        {
            if (dequeSize > 0)
            {
                return frontNode->data;
            } else
            {
                throw std::out_of_range("Deque is empty");
            }
        }

        [[nodiscard]] inline reference quick_front()
        {
            return frontNode->data;
        }

        [[nodiscard]] inline const_reference front() const
        {
            if (dequeSize > 0)
            {
                return frontNode->data;
            } else
            {
                throw std::out_of_range("Deque is empty");
            }
        }

        [[nodiscard]] inline const_reference quick_front() const
        {
            return frontNode->data;
        }

        [[nodiscard]] inline reference back()
        {
            if (dequeSize > 0)
            {
                return backNode->data;
            } else
            {
                throw std::out_of_range("Deque is empty");
            }
        }

        [[nodiscard]] inline reference quick_back()
        {
            return backNode->data;
        }

        [[nodiscard]] inline const_reference back() const
        {
            if (dequeSize > 0)
            {
                return backNode->data;
            } else
            {
                throw std::out_of_range("Deque is empty");
            }
        }

        [[nodiscard]] inline const_reference quick_back() const
        {
            return backNode->data;
        }

        void push_front(const_reference value)
        {
            if (dequeSize == 0 && frontNode == nullptr)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) DequeNode<TYPE>{value_type(value), nullptr, nullptr};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~DequeNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                dequeSize = 1;
                frontNode = workNode;
                backNode = workNode;
            } else
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) DequeNode<value_type>{value_type(value), nullptr, frontNode};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~DequeNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                frontNode->previous = workNode;
                frontNode = workNode;
                ++dequeSize;
            }
        }

        void push_front(value_type&& value)
        {
            if (dequeSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    return;
                }
                new(workNode) DequeNode<TYPE>{value_type(std::move(value)), nullptr, nullptr};
                dequeSize = 1;
                frontNode = workNode;
                backNode = workNode;
            } else
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    return;
                }
                new(workNode) DequeNode<TYPE>{value_type(std::move(value)), nullptr, frontNode};
                frontNode->previous = workNode;
                frontNode = workNode;
                ++dequeSize;
            }
        }

        template<typename... Args>
        void emplace_front(Args... args)
        {
            if (dequeSize == 0 && frontNode == nullptr)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) DequeNode<TYPE>{value_type(std::forward<Args>(args)...), nullptr, nullptr};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~DequeNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                dequeSize = 1;
                frontNode = workNode;
                backNode = workNode;
            } else
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) DequeNode<value_type>{value_type(std::forward<Args>(args)...), nullptr, frontNode};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~DequeNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                frontNode->previous = workNode;
                frontNode = workNode;
                ++dequeSize;
            }
        }

        void pop_front()
        {
            if (dequeSize == 1)
            {
                (*frontNode).~DequeNode<TYPE>();
                allocator.deallocate(frontNode, 1);
                dequeSize = 0;
                frontNode = nullptr;
                backNode = nullptr;
            } else if (dequeSize == 2)
            {
                (*frontNode).~DequeNode<TYPE>();
                allocator.deallocate(frontNode, 1);
                dequeSize = 1;
                backNode->previous = nullptr;
                frontNode = backNode;
            } else if (dequeSize > 2)
            {
                node_pointer nextNode = frontNode->next;
                (*frontNode).~DequeNode<TYPE>();
                allocator.deallocate(frontNode, 1);
                --dequeSize;
                nextNode->previous = nullptr;
                frontNode = nextNode;
            }
        }

        void push_back(const_reference value)
        {
            if (dequeSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) DequeNode<TYPE>{value_type(value), nullptr, nullptr};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~DequeNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                dequeSize = 1;
                frontNode = workNode;
                backNode = workNode;
            } else
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) DequeNode<value_type>{value_type(value), backNode, nullptr};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~DequeNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                backNode->next = workNode;
                backNode = workNode;
                ++dequeSize;
            }
        }

        void push_back(value_type&& value)
        {
            if (dequeSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    return;
                }
                new(workNode) DequeNode<TYPE>{value_type(std::move(value)), nullptr, nullptr};
                dequeSize = 1;
                frontNode = workNode;
                backNode = workNode;
            } else
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);

                } catch (std::bad_alloc)
                {
                    return;
                }

                new(workNode) DequeNode<value_type>{value_type(std::move(value)), backNode, nullptr};
                backNode->next = workNode;
                backNode = workNode;
                ++dequeSize;
            }
        }

        template<typename... Args>
        void emplace_back(Args... args)
        {
            if (dequeSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) DequeNode<TYPE>{value_type(std::forward<Args>(args)...), nullptr, nullptr};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~DequeNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                dequeSize = 1;
                frontNode = workNode;
                backNode = workNode;
            } else
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) DequeNode<value_type>{value_type(std::forward<Args>(args)...), backNode, nullptr};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~DequeNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                backNode->next = workNode;
                backNode = workNode;
                ++dequeSize;
            }
        }

        void pop_back()
        {
            if (dequeSize == 1)
            {
                (*frontNode).~DequeNode<TYPE>();
                allocator.deallocate(frontNode, 1);
                dequeSize = 0;
                frontNode = nullptr;
                backNode = nullptr;
            } else if (dequeSize == 2)
            {
                (*backNode).~DequeNode<TYPE>();
                allocator.deallocate(backNode, 1);
                dequeSize = 1;
                frontNode->next = nullptr;
                backNode = frontNode;
            } else if (dequeSize > 2)
            {
                node_pointer previousNode = backNode->previous;
                (*backNode).~DequeNode<TYPE>();
                allocator.deallocate(backNode, 1);
                --dequeSize;
                previousNode->next = nullptr;
                backNode = previousNode;
            }
        }

        void swap(DequeList<TYPE, ALLOCATOR>& other)
        {
            auto tempSize = other.dequeSize;
            auto tempFront = frontNode;
            auto tempBack = backNode;

            dequeSize = other.dequeSize;
            frontNode = other.frontNode;
            backNode = other.backNode;

            other.dequeSize = tempSize;
            other.frontNode = tempFront;
            other.backNode = tempBack;
        }

        [[nodiscard]] bool operator == (const DequeList<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal = true;
            if (dequeSize == other.dequeSize)
            {
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr)
                {
                    if (left->data != right->data)
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

        [[nodiscard]] bool operator != (const DequeList<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal = false;
            if (dequeSize == other.dequeSize)
            {
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr)
                {
                    if (left->data != right->data)
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

        [[nodiscard]] bool operator >= (const DequeList<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (dequeSize == other.dequeSize)
            {
                equal = true;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr)
                {
                    if (left->data != right->data)
                    {
                        if (left->data > right->data)
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
            } else if (dequeSize > other.dequeSize)
            {
                equal = true;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (right != nullptr)
                {
                    if (left->data != right->data)
                    {
                        if (left->data > right->data)
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

            } else if (dequeSize < other.dequeSize)
            {
                equal = false;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr)
                {
                    if (left->data != right->data)
                    {
                        if (left->data > right->data)
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

        [[nodiscard]] bool operator > (const DequeList<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (dequeSize == other.dequeSize)
            {
                equal = false;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr)
                {
                    if (left->data != right->data)
                    {
                        if (left->data > right->data)
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
            } else if (dequeSize > other.dequeSize)
            {
                equal = true;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (right != nullptr)
                {
                    if (left->data != right->data)
                    {
                        if (left->data > right->data)
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
            } else if (dequeSize < other.dequeSize)
            {
                equal = false;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr)
                {
                    if (left->data != right->data)
                    {
                        if (left->data > right->data)
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

        [[nodiscard]] bool operator <= (const DequeList<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (dequeSize == other.dequeSize)
            {
                equal = true;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr)
                {
                    if (left->data != right->data)
                    {
                        if (left->data < right->data)
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

            } else if (dequeSize > other.dequeSize)
            {
                equal = false;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (right != nullptr)
                {
                    if (left->data != right->data)
                    {
                        if (left->data < right->data)
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

            } else if (dequeSize < other.dequeSize)
            {
                equal = true;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr)
                {
                    if (left->data != right->data)
                    {
                        if (left->data < right->data)
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

        [[nodiscard]] bool operator < (const DequeList<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (dequeSize == other.dequeSize)
            {
                equal = false;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr)
                {
                    if (left->data != right->data)
                    {
                        if (left->data < right->data)
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
            } else if (dequeSize > other.dequeSize)
            {
                equal = false;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (right != nullptr)
                {
                    if (left->data != right->data)
                    {
                        if (left->data < right->data)
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

            } else if (dequeSize < other.dequeSize)
            {
                equal = true;
                node_pointer left = frontNode;
                node_pointer right = other.frontNode;

                while (left != nullptr)
                {
                    if (left->data != right->data)
                    {
                        if (left->data < right->data)
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
        size_type dequeSize;
        node_pointer frontNode;
        node_pointer backNode;
    };
}

#endif //INC_3DGAME_DEQUE_LIST_H
