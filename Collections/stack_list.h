#ifndef INC_3DGAME_STACK_LIST_H
#define INC_3DGAME_STACK_LIST_H

#include <stdexcept>
#include <cstdlib>
#include <cstdint>

namespace bice
{
    template<typename TYPE>
    struct StackNode
    {
        using value_type                   = TYPE;
        using reference                    = TYPE&;
        using const_reference              = const TYPE&;
        using pointer                      = TYPE*;
        using const_pointer                = const TYPE*;
        using node_pointer                 = StackNode<TYPE>*;
        using const_node_pointer           = const StackNode<TYPE>*;

        [[nodiscard]] inline bool operator == (const StackNode<TYPE>& other) const noexcept
        {
            return (data == other.data && next == other.next);
        }

        [[nodiscard]] inline bool operator != (const StackNode<TYPE>& other) const noexcept
        {
            return (data != other.data || next != other.next);
        }

        value_type data;
        node_pointer next;
    };

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    class StackList
    {
    public:

        using value_type                   = TYPE;
        using size_type                    = std::uint64_t;
        using difference_type              = std::int64_t;
        using allocator_type               = ALLOCATOR;
        using real_allocator_type          = typename std::allocator_traits<ALLOCATOR>::template rebind_alloc<StackNode<TYPE>>;
        using reference                    = TYPE&;
        using const_reference              = const TYPE&;
        using pointer                      = TYPE*;
        using const_pointer                = const TYPE*;
        using node_pointer                 = StackNode<TYPE>*;
        using const_node_pointer           = const StackNode<TYPE>*;

        StackList(allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        stackSize{}, topNode{}
        {}

        explicit StackList(const_reference value, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        stackSize{}, topNode{}
        {
            node_pointer workNode{nullptr};
            try
            {
                workNode = allocator.allocate(1);
                new(workNode) StackNode<TYPE>{value, nullptr};
            } catch (std::bad_alloc)
            {
                workNode = nullptr;
                return;
            } catch (...)
            {
                (*workNode).~StackNode<TYPE>();
                allocator.deallocate(workNode, 1);
                workNode = nullptr;
                return;
            }
            stackSize = 1;
            topNode = workNode;
        }

        StackList(std::initializer_list<value_type> list, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        stackSize{}, topNode{}
        {
            for(auto iter = list.begin(); iter != list.end(); ++iter)
            {
                push(*iter);
            }
        }

        template<typename ITERATOR>
        StackList(const ITERATOR begin, const ITERATOR end, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        stackSize{}, topNode{}
        {
            for(auto iter = begin; iter != end; ++iter)
            {
                push(*iter);
            }
        }

        StackList(const StackList<TYPE, ALLOCATOR>& other)
        {
            stackSize = other.stackSize;
            topNode = nullptr;
            if (stackSize > 0)
            {
                node_pointer currentNode = other.topNode;
                node_pointer workNode{nullptr};
                node_pointer previousNode = nullptr;

                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) StackNode<TYPE>{currentNode->data, nullptr};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    stackSize = 0;
                    return;
                } catch (...)
                {
                    (*workNode).~StackNode<TYPE>();
                    workNode = nullptr;
                    stackSize = 0;
                    return;
                }

                topNode = workNode;

                while (currentNode->next != nullptr)
                {
                    currentNode = currentNode->next;
                    previousNode = workNode;

                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) StackNode<TYPE>{currentNode->data, nullptr};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode =topNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~StackNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        topNode = nullptr;
                        stackSize = 0;
                        return;
                    } catch (...)
                    {
                        node_pointer currentNode =topNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~StackNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        topNode = nullptr;
                        stackSize = 0;
                        allocator.deallocate(workNode, 1);
                        return;
                    }

                    previousNode->next = workNode;
                }
            }
        }

        StackList<TYPE, ALLOCATOR>& operator = (const StackList<TYPE, ALLOCATOR>& other)
        {
            if (stackSize == other.stackSize)
            {
                node_pointer currentNode = other.topNode;
                node_pointer workNode = topNode;

                while (currentNode != nullptr)
                {
                    try
                    {
                        workNode->data = currentNode->data;
                    } catch (...)
                    {
                        node_pointer currentNode =topNode;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != nullptr)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~StackNode<TYPE>();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        topNode = nullptr;
                        stackSize = 0;
                        return *this;
                    }

                    workNode = workNode->next;
                    currentNode = currentNode->next;
                }
            } else
            {
                node_pointer currentNode = topNode;
                node_pointer deleteNode = currentNode;

                while (currentNode != nullptr)
                {
                    currentNode = currentNode->next;
                    (*deleteNode).~StackNode<TYPE>();
                    allocator.deallocate(deleteNode, 1);
                    deleteNode = currentNode;
                }

                stackSize = other.stackSize;
                topNode = nullptr;
                if (stackSize > 0)
                {
                    node_pointer currentNode = other.topNode;
                    node_pointer workNode{nullptr};
                    node_pointer previousNode = nullptr;

                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) StackNode<TYPE>{currentNode->data, nullptr};
                    } catch (std::bad_alloc)
                    {
                        workNode = nullptr;
                        stackSize = 0;
                        return *this;
                    } catch (...)
                    {
                        (*workNode).~StackNode<TYPE>();
                        workNode = nullptr;
                        stackSize = 0;
                        return *this;
                    }

                    topNode = workNode;

                    while (currentNode->next != nullptr)
                    {
                        currentNode = currentNode->next;
                        previousNode = workNode;

                        try
                        {
                            workNode = allocator.allocate(1);
                            new(workNode) StackNode<TYPE>{currentNode->data, nullptr};
                        } catch (std::bad_alloc)
                        {
                            node_pointer currentNode =topNode;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != nullptr)
                            {
                                currentNode = currentNode->next;
                                (*deleteNode).~StackNode<TYPE>();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }
                            topNode = nullptr;
                            stackSize = 0;
                            return *this;
                        } catch (...)
                        {
                            node_pointer currentNode =topNode;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != nullptr)
                            {
                                currentNode = currentNode->next;
                                (*deleteNode).~StackNode<TYPE>();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }
                            topNode = nullptr;
                            stackSize = 0;
                            (*workNode).~StackNode<TYPE>();
                            allocator.deallocate(workNode, 1);
                            return *this;
                        }

                        previousNode->next = workNode;
                    }
                }
            }
            return *this;
        }

        StackList(StackList<TYPE, ALLOCATOR>&& other) noexcept :
        stackSize(std::move(other.stackSize)),
        topNode(std::move(other.topNode))
        {
            other.topNode = nullptr;
        }

        StackList<TYPE, ALLOCATOR>& operator = (StackList<TYPE, ALLOCATOR>&& other) noexcept
        {
            node_pointer currentNode = topNode;
            node_pointer deleteNode = currentNode;

            while (currentNode != nullptr)
            {
                currentNode = currentNode->next;
                (*deleteNode).~StackNode<TYPE>();
                allocator.deallocate(deleteNode, 1);
                deleteNode = currentNode;
            }
            stackSize = std::move(other.stackSize);
            topNode = std::move(other.topNode);

            other.topNode = nullptr;

            return *this;
        }

        virtual ~StackList()
        {
            node_pointer currentNode = topNode;
            node_pointer deleteNode = currentNode;

            while (currentNode != nullptr)
            {
                currentNode = currentNode->next;
                (*deleteNode).~StackNode<TYPE>();
                allocator.deallocate(deleteNode, 1);
                deleteNode = currentNode;
            }
        }

        [[nodiscard]] inline size_type size() const noexcept
        {
            return stackSize;
        }

        [[nodiscard]] inline bool empty() const noexcept
        {
            return stackSize == 0;
        }

        [[nodiscard]] reference top()
        {
            if (stackSize > 0)
            {
                return topNode->data;
            } else
            {
                throw std::out_of_range("Stack is empty");
            }
        }

        [[nodiscard]] reference quick_top()
        {
            return topNode->data;
        }

        [[nodiscard]] const_reference top() const
        {
            if (stackSize > 0)
            {
                return topNode->data;
            } else
            {
                throw std::out_of_range("Stack is empty");
            }
        }

        [[nodiscard]] const_reference quick_top() const
        {
            return topNode->data;
        }

        inline void push(const_reference value)
        {
            if (stackSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) StackNode<TYPE>{value, nullptr};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~StackNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    return;
                }
                topNode = workNode;
                stackSize = 1;
            } else
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) StackNode<TYPE>{value, topNode};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~StackNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    return;
                }
                ++stackSize;
                topNode = workNode;
            }
        }

        inline void push(value_type&& value)
        {
            if (stackSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) StackNode<TYPE>{std::move(value), nullptr};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                }
                topNode = workNode;
                stackSize = 1;
            } else
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) StackNode<TYPE>{std::move(value), topNode};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                }
                ++stackSize;
                topNode = workNode;
            }
        }

        template<typename... Args>
        void emplace(Args... args)
        {
            if (stackSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) StackNode<TYPE>{value_type(std::forward<Args>(args)...), nullptr};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~StackNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    return;
                }
                topNode = workNode;
                stackSize = 1;
            } else
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) StackNode<TYPE>{value_type(std::forward<Args>(args)...), topNode};
                } catch (std::bad_alloc)
                {
                    workNode = nullptr;
                    return;
                } catch (...)
                {
                    (*workNode).~StackNode<TYPE>();
                    allocator.deallocate(workNode, 1);
                    workNode = nullptr;
                    return;
                }
                ++stackSize;
                topNode = workNode;
            }
        }

        void pop()
        {
            if (stackSize > 0)
            {
                node_pointer nextNode = topNode->next;
                (*topNode).~StackNode<TYPE>();
                --stackSize;
                topNode = nextNode;
            }
        }

        void quick_pop()
        {
            node_pointer nextNode = topNode->next;
            (*topNode).~StackNode<TYPE>();
            --stackSize;
            topNode = nextNode;
        }

        void swap(StackList<TYPE, ALLOCATOR>& other)
        {
            const auto tempSize = stackSize;
            const auto tempNode = topNode;

            stackSize = other.stackSize;
            topNode = other.topNode;

            other.topNode = tempNode;
            other.stackSize = tempSize;
        }

        [[nodiscard]] bool operator == (const StackList<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal = true;
            if (stackSize == other.stackSize)
            {
                node_pointer left = topNode;
                node_pointer right = other.topNode;

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

        [[nodiscard]] bool operator != (const StackList<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal = false;
            if (stackSize == other.stackSize)
            {
                node_pointer left = topNode;
                node_pointer right = other.topNode;

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

        [[nodiscard]] bool operator >= (const StackList<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (stackSize == other.stackSize)
            {
                equal = true;
                node_pointer left = topNode;
                node_pointer right = other.topNode;

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
            } else if (stackSize > other.stackSize)
            {
                equal = true;
                node_pointer left = topNode;
                node_pointer right = other.topNode;

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

            } else if (stackSize < other.stackSize)
            {
                equal = false;
                node_pointer left = topNode;
                node_pointer right = other.topNode;

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

        [[nodiscard]] bool operator > (const StackList<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (stackSize == other.stackSize)
            {
                equal = false;
                node_pointer left = topNode;
                node_pointer right = other.topNode;

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
            } else if (stackSize > other.stackSize)
            {
                equal = true;
                node_pointer left = topNode;
                node_pointer right = other.topNode;

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
            } else if (stackSize < other.stackSize)
            {
                equal = false;
                node_pointer left = topNode;
                node_pointer right = other.topNode;

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

        [[nodiscard]] bool operator <= (const StackList<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (stackSize == other.stackSize)
            {
                equal = true;
                node_pointer left = topNode;
                node_pointer right = other.topNode;

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

            } else if (stackSize > other.stackSize)
            {
                equal = false;
                node_pointer left = topNode;
                node_pointer right = other.topNode;

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

            } else if (stackSize < other.stackSize)
            {
                equal = true;
                node_pointer left = topNode;
                node_pointer right = other.topNode;

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

        [[nodiscard]] bool operator < (const StackList<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (stackSize == other.stackSize)
            {
                equal = false;
                node_pointer left = topNode;
                node_pointer right = other.topNode;

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
            } else if (stackSize > other.stackSize)
            {
                equal = false;
                node_pointer left = topNode;
                node_pointer right = other.topNode;

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

            } else if (stackSize < other.stackSize)
            {
                equal = true;
                node_pointer left = topNode;
                node_pointer right = other.topNode;

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
        size_type stackSize;
        node_pointer topNode;
    };


}

#endif //INC_3DGAME_STACK_LIST_H
