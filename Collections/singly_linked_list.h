#ifndef INC_3DGAME_SINGLY_LINKED_LIST_H
#define INC_3DGAME_SINGLY_LINKED_LIST_H

#include <stdexcept>
#include <cstdlib>
#include <initializer_list>
#include <cassert>
#include <iostream>
#include <iterator>
#include <array>
#include <utility>

namespace bice
{
    template<typename TYPE>
    struct SinglyListNode
    {
        using value_type                               = TYPE;
        using reference                                = TYPE&;
        using const_reference                          = const TYPE&;
        using pointer                                  = TYPE*;
        using const_pointer                            = const TYPE*;
        using node_pointer                             = SinglyListNode<TYPE>*;
        using const_node_pointer                       = const SinglyListNode<TYPE>*;

        [[nodiscard]] inline bool operator == (const SinglyListNode<TYPE>& other) const noexcept
        {
            return data == other.data && next == other.next;
        }

        [[nodiscard]] inline bool operator != (const SinglyListNode<TYPE>& other) const noexcept
        {
            return data != other.data || next != other.next;
        }

        value_type data;
        node_pointer next;
    };

    template<typename TYPE>
    struct ConstSinglyListIterator
    {
        using iterator_category                        = std::forward_iterator_tag;
        using value_type                               = TYPE;
        using difference_type                          = std::int64_t;
        using reference                                = const TYPE&;
        using pointer                                  = const TYPE*;
        using iterator                                 = ConstSinglyListIterator<value_type>;
        using node_type                                = SinglyListNode<value_type>;
        using node_pointer                             = SinglyListNode<value_type>*;

        node_pointer node;

        ConstSinglyListIterator() noexcept :
        node{}
        {}

        ConstSinglyListIterator(node_pointer node_) noexcept
        : node(node_)
        {}

        reference operator * () const noexcept
        {
            return node->data;
        }

        pointer operator -> () const noexcept
        {
            return static_cast<pointer>(&(node->data));
        }

        iterator& operator ++ () noexcept
        {
            node = node->next;
            return *this;
        }

        iterator operator ++ (int) noexcept
        {
            iterator temp(*this);
            node = node->next;
            return temp;
        }

        bool operator == (const iterator& other) const noexcept
        {
            return *node == *(other.node);
        }

        bool operator != (const iterator& other) const noexcept
        {
            return *node != *(other.node);
        }

        iterator next() const noexcept
        {
            if (this->node)
                return iterator(node->next);
            else
                return iterator(nullptr);
        }
    };

    template<typename TYPE>
    struct SinglyListIterator
    {
        using iterator_category                        = std::forward_iterator_tag;
        using value_type                               = TYPE;
        using difference_type                          = std::int64_t;
        using reference                                = TYPE&;
        using pointer                                  = TYPE*;
        using iterator                                 = SinglyListIterator<value_type>;
        using node_type                                = SinglyListNode<value_type>;
        using node_pointer                             = SinglyListNode<value_type>*;

        node_pointer node;

        SinglyListIterator() noexcept :
        node{}
        {}

        explicit SinglyListIterator(node_pointer node_) noexcept :
        node(node_)
        {}

        reference operator * () noexcept
        {
            return node->data;
        }

        pointer operator -> () noexcept
        {
            return &node->data;
        }

        iterator& operator ++ () noexcept
        {
            node = node->next;
            return *this;
        }

        iterator operator ++(int) noexcept
        {
            iterator temp(*this);
            node = node->_M_next;
            return temp;
        }

        bool operator == (const iterator& other) const noexcept
        {
            return *node == *(other.node);
        }

        bool operator != (const iterator& other) const noexcept
        {
            return *node != *other.node;
        }

        iterator next() const noexcept
        {
            if (node)
                return iterator(node->next);
            else
                return iterator(nullptr);
        }
    };

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    class SinglyList
    {
    public:

        using value_type                               = TYPE;
        using size_type                                = std::uint64_t;
        using difference_type                          = std::int64_t;
        using allocator_type                           = ALLOCATOR;
        using real_allocator_type                      = typename std::allocator_traits<ALLOCATOR>::template rebind_alloc<SinglyListNode<TYPE>>;
        using reference                                = TYPE&;
        using const_reference                          = const TYPE&;
        using pointer                                  = TYPE*;
        using const_pointer                            = const TYPE*;
        using node_type                                = SinglyListNode<value_type>;
        using node_pointer                             = SinglyListNode<TYPE>*;
        using const_node_pointer                       = const SinglyListNode<TYPE>*;
        using iterator                                 = SinglyListIterator<value_type>;
        using const_iterator                           = ConstSinglyListIterator<value_type>;

        SinglyList(allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        listSize{},
        headNode{}, tailNode{}
        {
            initList();
        }

        explicit SinglyList(const size_type size, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        listSize{},
        headNode{}, tailNode{}
        {
            initList();

            if (size > 0)
            {
                node_pointer workNode{nullptr};
                node_pointer nextNode = tailNode;

                for(size_type idx = 0; idx < size; ++idx)
                {
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) node_type{value_type(), nextNode};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = headNode->next;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        return;
                    } catch (...)
                    {

                        node_pointer currentNode = headNode->next;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }

                        (*workNode).~node_type();
                        allocator.deallocate(workNode, 1);
                        return;
                    }
                    nextNode = workNode;
                }
                headNode->next = workNode;
                listSize = size;
            }
        }

        explicit SinglyList(const size_type size, const_reference value, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        listSize{},
        headNode{}, tailNode{}
        {
            initList();

            if (size > 0)
            {
                node_pointer workNode{nullptr};
                node_pointer nextNode = tailNode;

                for(size_type idx = 0; idx < size; ++idx)
                {
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) node_type{value_type(value), nextNode};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = headNode->next;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        return;
                    } catch (...)
                    {

                        node_pointer currentNode = headNode->next;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }

                        (*workNode).~node_type();
                        allocator.deallocate(workNode, 1);
                        return;
                    }
                    nextNode = workNode;
                }
                headNode->next = workNode;
                listSize = size;
            }
        }

        SinglyList(std::initializer_list<value_type> list, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        listSize{},
        headNode{}, tailNode{}
        {
            initList();
            if (list.size() > 0)
            {
                node_pointer workNode{nullptr};
                node_pointer previousNode = headNode;

                for(auto iter = list.begin(); iter != list.end(); ++iter)
                {
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) node_type{value_type(*iter), nullptr};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = headNode->next;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        return;
                    } catch (...)
                    {

                        node_pointer currentNode = headNode->next;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }

                        (*workNode).~node_type();
                        allocator.deallocate(workNode, 1);
                        return;
                    }
                    previousNode->next = workNode;
                    previousNode = workNode;
                }
                workNode->next = tailNode;
                listSize = list.size();
            }
        }

        template<typename ITERATOR>
        SinglyList(const ITERATOR begin, const ITERATOR end, allocator_type allocator_ = ALLOCATOR())
        {
            initList();

            difference_type distance{0};
            for(auto iter = begin; iter != end; ++iter)
            {
                ++distance;
            }

            if (distance > 0)
            {
                node_pointer workNode{nullptr};
                node_pointer previousNode = headNode;

                for(auto iter = begin; iter != end; ++iter)
                {
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) node_type{value_type(*iter), nullptr};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = headNode->next;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        return;
                    } catch (...)
                    {

                        node_pointer currentNode = headNode->next;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }

                        (*workNode).~node_type();
                        allocator.deallocate(workNode, 1);
                        return;
                    }
                    previousNode->next = workNode;
                    previousNode = workNode;
                }
                workNode->next = tailNode;
                listSize = distance;
            }
        }

        SinglyList(const SinglyList<TYPE, ALLOCATOR>& other)
        {
            listSize = other.listSize;
            headNode = nullptr;
            tailNode = nullptr;
            initList();
            if (listSize > 0)
            {
                node_pointer otherNode = other.headNode->next;
                node_pointer workNode{nullptr};
                node_pointer previousNode = headNode;

                for(size_type idx = 0; idx < listSize; ++idx)
                {
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) node_type{value_type(otherNode->data), nullptr};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = headNode->next;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        listSize = 0;
                        headNode->next = tailNode;
                        return;
                    } catch (...)
                    {
                        node_pointer currentNode = headNode->next;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        (*workNode).~node_type();
                        allocator.deallocate(workNode, 1);
                        listSize = 0;
                        headNode->next = tailNode;
                    }
                    otherNode = otherNode->next;
                    previousNode->next = workNode;
                    previousNode = workNode;
                }
                workNode->next = tailNode;
            }
        }

        SinglyList<TYPE, ALLOCATOR>& operator = (const SinglyList<TYPE, ALLOCATOR>& other)
        {
            if (listSize == other.listSize && listSize > 0)
            {
                node_pointer otherNode = other.headNode->next;
                node_pointer currentNode = tailNode->next;

                while (currentNode != tailNode)
                {
                    try
                    {
                        currentNode->data = otherNode->data;
                    } catch (...)
                    {
                        node_pointer currentNode = headNode->next;
                        node_pointer deleteNode = headNode->next;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        listSize = 0;
                        return *this;
                    }
                    currentNode = currentNode->next;
                    otherNode = otherNode->next;
                }

            } else
            {
                node_pointer currentNode = headNode->next;
                node_pointer deleteNode = headNode->next;

                while (currentNode != tailNode)
                {
                    currentNode = currentNode->next;
                    (*deleteNode).~node_type();
                    allocator.deallocate(deleteNode, 1);
                    deleteNode = currentNode;
                }

                listSize = other.listSize;
                headNode->next = tailNode;
                if (listSize > 0)
                {
                    node_pointer otherNode = other.headNode->next;
                    node_pointer workNode{nullptr};
                    node_pointer previousNode = headNode;

                    for(size_type idx = 0; idx < listSize; ++idx)
                    {
                        try
                        {
                            workNode = allocator.allocate(1);
                            new(workNode) node_type{value_type(otherNode->data), nullptr};
                        } catch (std::bad_alloc)
                        {
                            node_pointer currentNode = headNode->next;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != tailNode)
                            {
                                currentNode = currentNode->next;
                                (*deleteNode).~node_type();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }
                            listSize = 0;
                            headNode->next = tailNode;
                            return *this;
                        } catch (...)
                        {
                            node_pointer currentNode = headNode->next;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != tailNode)
                            {
                                currentNode = currentNode->next;
                                (*deleteNode).~node_type();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }
                            (*workNode).~node_type();
                            allocator.deallocate(workNode, 1);
                            listSize = 0;
                            headNode->next = tailNode;
                        }
                        otherNode = otherNode->next;
                        previousNode->next = workNode;
                        previousNode = workNode;
                    }
                    workNode->next = tailNode;
                }
            }
            return *this;
        }

        SinglyList(SinglyList<TYPE, ALLOCATOR>&& other) noexcept :
        listSize(std::move(other.listSize)),
        headNode(std::move(other.headNode)),
        tailNode(std::move(other.tailNode))
        {
            other.listSize = 0;
            other.headNode = nullptr;
            other.tailNode = nullptr;
        }

        SinglyList<TYPE, ALLOCATOR>& operator = (SinglyList<TYPE, ALLOCATOR>&& other) noexcept
        {
            node_pointer currentNode = headNode;
            node_pointer deleteNode = headNode;

            while (currentNode != nullptr)
            {
                currentNode = currentNode->next;
                (*deleteNode).~node_type();
                allocator.deallocate(deleteNode, 1);
                deleteNode = currentNode;
            }

            listSize = std::move(other.listSize);
            headNode = std::move(other.headNode);
            tailNode = std::move(other.tailNode);

            other.listSize = 0;
            other.headNode = nullptr;
            other.tailNode = nullptr;

            return *this;
        }

        virtual ~SinglyList()
        {
            node_pointer currentNode = headNode;
            node_pointer deleteNode = headNode;

            while (currentNode != nullptr)
            {
                currentNode = currentNode->next;
                (*deleteNode).~node_type();
                allocator.deallocate(deleteNode, 1);
                deleteNode = currentNode;
            }
        }

        [[nodiscard]] inline size_type size() const noexcept
        {
            return listSize;
        }

        [[nodiscard]] inline size_type max_size() const noexcept
        {
            return std::numeric_limits<size_type>::max();
        }

        [[nodiscard]] inline bool empty() const noexcept
        {
            return listSize == 0;
        }

        [[nodiscard]] inline reference front()
        {
            if (listSize > 0)
            {
                return headNode->next->data;
            } else
            {
                throw std::out_of_range("List is empty");
            }
        }

        [[nodiscard]] inline const_reference front() const
        {
            if (listSize > 0)
            {
                return headNode->next->data;
            } else
            {
                throw std::out_of_range("List is empty");
            }
        }

        [[nodiscard]] inline reference quick_front()
        {
            return headNode->next->data;
        }

        [[nodiscard]] inline const_reference quick_front() const
        {
            return headNode->next->data;
        }

        [[nodiscard]] inline iterator before_begin()
        {
            return iterator(headNode);
        }

        [[nodiscard]] inline const_iterator before_begin() const
        {
            return const_iterator(headNode);
        }

        [[nodiscard]] inline const_iterator cbefore_begin() const
        {
            return const_iterator(headNode);
        }

        [[nodiscard]] inline iterator begin() noexcept
        {
            return iterator(headNode->next);
        }

        [[nodiscard]] inline const_iterator begin() const noexcept
        {
            return const_iterator(headNode->next);
        }

        [[nodiscard]] inline const_iterator cbegin() const noexcept
        {
            return const_iterator(headNode->next);
        }

        [[nodiscard]] inline iterator end() noexcept
        {
            return iterator(tailNode);
        }

        [[nodiscard]] inline const_iterator end() const noexcept
        {
            return const_iterator(tailNode);
        }

        [[nodiscard]] inline const_iterator cend() const noexcept
        {
            return const_iterator(tailNode);
        }

        void assign(const size_type count, const_reference value)
        {
            if (count >= 0)
            {
                if (count == listSize && count > 0)
                {
                    node_pointer currentNode = headNode->next;

                    while (currentNode != tailNode)
                    {
                        try
                        {
                            currentNode->data = value;
                        } catch (...)
                        {
                            node_pointer currentNode = headNode->next;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != tailNode)
                            {
                                currentNode = currentNode->next;
                                (*deleteNode).~node_type();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }
                            listSize = 0;
                            headNode->next = tailNode;
                            return;
                        }
                    }
                } else
                {
                    node_pointer currentNode = headNode->next;
                    node_pointer deleteNode = currentNode;

                    while (currentNode != tailNode)
                    {
                        currentNode = currentNode->next;
                        (*deleteNode).~node_type();
                        allocator.deallocate(deleteNode, 1);
                        deleteNode = currentNode;
                    }

                    listSize = 0;
                    headNode->next = tailNode;

                    if (count > 0)
                    {
                        node_pointer workNode{nullptr};
                        node_pointer nextNode = tailNode;

                        for(size_type idx = 0; idx < count; ++idx)
                        {
                            try
                            {
                                workNode = allocator.allocate(1);
                                new(workNode) node_type{value_type(value), nextNode};
                            } catch (std::bad_alloc)
                            {
                                node_pointer currentNode = headNode->next;
                                node_pointer deleteNode = currentNode;

                                while (currentNode != tailNode)
                                {
                                    currentNode = currentNode->next;
                                    (*deleteNode).~node_type();
                                    allocator.deallocate(deleteNode, 1);
                                    deleteNode = currentNode;
                                }
                                return;
                            } catch (...)
                            {

                                node_pointer currentNode = headNode->next;
                                node_pointer deleteNode = currentNode;

                                while (currentNode != tailNode)
                                {
                                    currentNode = currentNode->next;
                                    (*deleteNode).~node_type();
                                    allocator.deallocate(deleteNode, 1);
                                    deleteNode = currentNode;
                                }

                                (*workNode).~node_type();
                                allocator.deallocate(workNode, 1);
                                return;
                            }
                            nextNode = workNode;
                        }
                        headNode->next = workNode;
                        listSize = count;
                    }
                }
            }
        }

        template<typename ITERATOR>
        void assign(const ITERATOR first, const ITERATOR last)
        {
            difference_type distance{0};
            for(auto iter = first; iter != last; ++iter)
            {
                ++distance;
            }

            if (distance == listSize && listSize > 0)
            {
                node_pointer currentNode = headNode->next;
                for(auto iter = first; iter != last; ++iter)
                {
                    try
                    {
                        currentNode->data = *iter;
                    } catch (...)
                    {
                        node_pointer currentNode = headNode->next;
                        node_pointer deleteNode = currentNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        listSize = 0;
                        headNode->next = tailNode;
                        return;
                    }
                    currentNode = currentNode->next;

                }
            } else
            {
                node_pointer currentNode = headNode->next;
                node_pointer deleteNode = currentNode;

                while (currentNode != tailNode)
                {
                    currentNode = currentNode->next;
                    (*deleteNode).~node_type();
                    allocator.deallocate(deleteNode, 1);
                    deleteNode = currentNode;
                }

                listSize = 0;
                headNode->next = tailNode;

                if (distance > 0)
                {
                    node_pointer workNode{nullptr};
                    node_pointer previousNode = headNode;

                    for(auto iter = first; iter != last; ++iter)
                    {
                        try
                        {
                            workNode = allocator.allocate(1);
                            new(workNode) node_type{value_type(*iter), nullptr};
                        } catch (std::bad_alloc)
                        {
                            node_pointer currentNode = headNode->next;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != tailNode)
                            {
                                currentNode = currentNode->next;
                                (*deleteNode).~node_type();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }
                            return;
                        } catch (...)
                        {

                            node_pointer currentNode = headNode->next;
                            node_pointer deleteNode = currentNode;

                            while (currentNode != tailNode)
                            {
                                currentNode = currentNode->next;
                                (*deleteNode).~node_type();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }

                            (*workNode).~node_type();
                            allocator.deallocate(workNode, 1);
                            return;
                        }
                        previousNode->next = workNode;
                        previousNode = workNode;
                    }
                    workNode->next = tailNode;
                    listSize = distance;
                }
            }
        }

        void push_front(const_reference value)
        {
            if (listSize == 0 && headNode == nullptr)
            {
                initList();
            }

            node_pointer workNode{nullptr};
            node_pointer nextNode = headNode->next;

            try
            {
                workNode = allocator.allocate(1);
                new(workNode) node_type{value_type(value), nextNode};
            } catch (std::bad_alloc)
            {
                return;
            } catch (...)
            {
                (*workNode).~node_type();
                allocator.deallocate(workNode, 1);
                return;
            }
            headNode->next = workNode;
            ++listSize;
        }

        void push_front(value_type&& value)
        {
            if (listSize == 0 && headNode == nullptr)
            {
                initList();
            }

            node_pointer workNode{nullptr};
            node_pointer nextNode = headNode->next;

            try
            {
                workNode = allocator.allocate(1);
            } catch (std::bad_alloc)
            {
                return;
            }

            new(workNode) node_type{value_type(std::move(value)), nextNode};
            headNode->next = workNode;
            ++listSize;
        }

        template<typename... Args>
        void emplace_front(Args... args)
        {
            if (listSize == 0 && headNode == nullptr)
            {
                initList();
            }

            node_pointer workNode{nullptr};
            node_pointer nextNode = headNode->next;

            try
            {
                workNode = allocator.allocate(1);
                new(workNode) node_type{value_type(std::forward<Args>(args)...), nextNode};
            } catch (std::bad_alloc)
            {
                return;
            } catch (...)
            {
                (*workNode).~node_type();
                allocator.deallocate(workNode, 1);
                return;
            }
            headNode->next = workNode;
            ++listSize;
        }

        void pop_front()
        {
            if (listSize > 0)
            {
                node_pointer frontNode = headNode->next;
                node_pointer nextNode = frontNode->next;
                (*frontNode).~node_type();
                allocator.deallocate(frontNode, 1);
                --listSize;
                headNode->next = nextNode;
            }
        }

        iterator erase_after(const_iterator position)
        {
            /*const_iterator deleteNode = position.next();
            const_iterator next = deleteNode.next();

            (*deleteNode.node).~node_type();
            allocator.deallocate(deleteNode.node, 1);
            position.node->next = next.node;
            --listSize;
            return iterator(next.node);*/

            if (listSize > 0 && position != tailNode)
            {
                const_iterator deleteNode = position.next();
                const_iterator next = deleteNode.next();

                (*deleteNode.node).~node_type();
                allocator.deallocate(deleteNode.node, 1);
                position.node->next = next.node;
                --listSize;
                return iterator(next.node);
            }
            return iterator(tailNode);
        }

        iterator erase_after(const_iterator first, const_iterator last)
        {
            difference_type distance{0};
            for(auto iter = first; iter != last; ++iter)
            {
                ++distance;
            }

            if (distance > 0)
            {
                node_pointer currentNode = first.node->next;
                node_pointer deleteNode = currentNode;

                while (currentNode != last.node)
                {
                    currentNode = currentNode->next;
                    (*deleteNode).~node_type();
                    allocator.deallocate(deleteNode, 1);
                    deleteNode = currentNode;
                }
                listSize -= distance;
                first.node->next = last.node;
                return iterator(last.node);
            }
            return iterator(tailNode);
        }

        template<typename... Args>
        iterator emplace_after(const_iterator pos, Args... args)
        {
            node_pointer workNode{nullptr};
            node_pointer nextNode = pos.node->next;

            try
            {
                workNode = allocator.allocate(1);
                new(workNode) node_type{value_type(std::forward<Args>(args)...), nextNode};
            } catch (std::bad_alloc)
            {
                return iterator(tailNode);
            } catch (...)
            {
                (*workNode).~node_type();
                allocator.deallocate(workNode, 1);
                return iterator(tailNode);
            }

            ++listSize;
            pos.node->next = workNode;
            return iterator(workNode);
        }

        iterator insert_after(const_iterator pos, const_reference value)
        {
            node_pointer workNode{nullptr};
            node_pointer nextNode = pos.node->next;

            try
            {
                workNode = allocator.allocate(1);
                new(workNode) node_type{value_type(value), nextNode};
            } catch (std::bad_alloc)
            {
                return iterator(tailNode);
            } catch (...)
            {
                (*workNode).~node_type();
                allocator.deallocate(workNode, 1);
                return iterator(tailNode);
            }

            ++listSize;
            pos.node->next = workNode;
            return iterator(workNode);
        }

        iterator insert_after(const_iterator pos, value_type&& value)
        {
            node_pointer workNode{nullptr};
            node_pointer nextNode = pos.node->next;

            try
            {
                workNode = allocator.allocate(1);
            } catch (std::bad_alloc)
            {
                return iterator(tailNode);
            }

            new(workNode) node_type{value_type(std::move(value)), nextNode};

            ++listSize;
            pos.node->next = workNode;
            return iterator(workNode);
        }

        iterator insert_after(const_iterator pos, size_type count, const_reference value)
        {
            node_pointer workNode{nullptr};
            node_pointer nextNode = pos.node->next;
            node_pointer previousNode = pos.node;

            for (int idx = 0; idx < count; ++idx)
            {
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(value), nullptr};
                } catch (std::bad_alloc)
                {
                    node_pointer currentNode = pos.node->next;
                    node_pointer deleteNode = currentNode;

                    while (currentNode != nextNode)
                    {
                        currentNode = currentNode->next;
                        (*deleteNode).~node_type();
                        allocator.deallocate(deleteNode, 1);
                        deleteNode = currentNode;
                    }
                    pos.node->next = nextNode;
                    return iterator(tailNode);
                } catch (...)
                {
                    node_pointer currentNode = pos.node->next;
                    node_pointer deleteNode = currentNode;

                    while (currentNode != nextNode)
                    {
                        currentNode = currentNode->next;
                        (*deleteNode).~node_type();
                        allocator.deallocate(deleteNode, 1);
                        deleteNode = currentNode;
                    }
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    pos.node->next = nextNode;
                    return iterator(tailNode);
                }
                previousNode->next = workNode;
                previousNode = workNode;
            }
            workNode->next = nextNode;
            listSize += count;
            return iterator(workNode);
        }

        void clear()
        {
            if (listSize > 0)
            {
                node_pointer currentNode = headNode->next;
                node_pointer deleteNode = currentNode;

                while (currentNode != tailNode)
                {
                    currentNode = currentNode->next;
                    (*deleteNode).~node_type();
                    allocator.deallocate(deleteNode);
                    deleteNode = currentNode;
                }

                headNode->next = tailNode;
                listSize = 0;
            }
        }

        void resize(const size_type& count)
        {}

        void resize(const size_type& count, const_reference value)
        {}

        void swap(SinglyList<TYPE, ALLOCATOR>& other)
        {
            const auto tempSize = listSize;
            const auto tempHead = headNode;
            const auto tempEnd = tailNode;

            listSize = other.listSize;
            headNode = other.headNode;
            tailNode = other.tailNode;

            other.listSize = tempSize;
            other.headNode = tempHead;
            other.tailNode = tempEnd;
        }

        void merge(SinglyList<TYPE, ALLOCATOR>& other)
        {}

        void merge(SinglyList<TYPE, ALLOCATOR>&& other)
        {}

        template<typename Compare>
        void merge(SinglyList<TYPE, ALLOCATOR>& other, Compare compare)
        {}

        template<typename Compare>
        void merge(SinglyList<TYPE, ALLOCATOR>&& other, Compare compare)
        {}

        void splice_after(const_iterator position, SinglyList<TYPE, ALLOCATOR>& other)
        {}

        void splice_after(const_iterator position, SinglyList<TYPE, ALLOCATOR>&& other)
        {}

        void splice_after(const_iterator position, SinglyList<TYPE, ALLOCATOR>& other, const_iterator it)
        {}

        void splice_after(const_iterator position, SinglyList<TYPE, ALLOCATOR>&& other, const_iterator it)
        {}

        void splice_after(const_iterator position, SinglyList<TYPE, ALLOCATOR>& other,
                          const_iterator first, const_iterator last)
        {}

        void splice_after(const_iterator position, SinglyList<TYPE, ALLOCATOR>&& other,
                          const_iterator first, const_iterator last)
        {}

        void remove(const_reference value)
        {}

        template<typename UnaryPredicate>
        void remove_if(UnaryPredicate unaryPredicate)
        {}

        void reverse()
        {}

        void unique()
        {}

        template<typename BinaryPredicate>
        void unique(BinaryPredicate binaryPredicate)
        {}

        void select_sort()
        {
            if (listSize > 0)
            {
                auto swap = [](node_pointer a, node_pointer b)
                {
                    value_type data = a->data;
                    a->data = b->data;
                    b->data = data;
                };

                node_pointer min;
                node_pointer iter_1 = headNode->next;
                node_pointer iter_2;
                for(size_type index_1 = 0; index_1 < listSize - 1; ++index_1)
                {
                    min = iter_1;
                    iter_2 = iter_1->next;
                    for(size_type index_2 = index_1 + 1; index_2 < listSize; ++index_2)
                    {
                        if (min->data > iter_2->data)
                        {
                            min = iter_2;
                        }
                        iter_2 = iter_2->next;
                    }
                    swap(min, iter_1);
                    iter_1 = iter_1->next;
                }
            }
        }

        void sort()
        {
            merge_sort(&(headNode->next));
        }

        [[nodiscard]] bool operator == (const SinglyList<TYPE, ALLOCATOR>& other)
        {
            bool equal = true;
            if (listSize == other.listSize)
            {
                node_pointer left = headNode->next;
                node_pointer right = other.headNode->next;

                while (left != tailNode)
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

        [[nodiscard]] bool operator != (const SinglyList<TYPE, ALLOCATOR>& other)
        {
            bool equal = false;
            if (listSize == other.listSize)
            {
                node_pointer left = headNode->next;
                node_pointer right = other.headNode->next;

                while (left != tailNode)
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

        [[nodiscard]] bool operator >= (const SinglyList<TYPE, ALLOCATOR>& other)
        {
            bool equal;

            if (listSize == other.listSize)
            {
                equal = true;
                node_pointer left = headNode->next;
                node_pointer right = other.headNode->next;

                while (left != tailNode)
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
            } else if (listSize > other.listSize)
            {
                equal = true;
                node_pointer left = headNode->next;
                node_pointer right = other.headNode->next;

                while (right != other.tailNode)
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

            } else if (listSize < other.listSize)
            {
                equal = false;
                node_pointer left = headNode->next;
                node_pointer right = other.headNode->next;

                while (left != tailNode)
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

        [[nodiscard]] bool operator > (const SinglyList<TYPE, ALLOCATOR>& other)
        {
            bool equal;

            if (listSize == other.listSize)
            {
                equal = false;
                node_pointer left = headNode->next;
                node_pointer right = other.headNode->next;

                while (left != tailNode)
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
            } else if (listSize > other.listSize)
            {
                equal = true;
                node_pointer left = headNode->next;
                node_pointer right = other.headNode->next;

                while (right != other.tailNode)
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
            } else if (listSize < other.listSize)
            {
                equal = false;
                node_pointer left = headNode->next;
                node_pointer right = other.headNode->next;

                while (left != tailNode)
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

        [[nodiscard]] bool operator <= (const SinglyList<TYPE, ALLOCATOR>& other)
        {
            bool equal;

            if (listSize == other.listSize)
            {
                equal = true;
                node_pointer left = headNode->next;
                node_pointer right = other.headNode->next;

                while (left != tailNode)
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

            } else if (listSize > other.listSize)
            {
                equal = false;
                node_pointer left = headNode->next;
                node_pointer right = other.headNode->next;

                while (right != other.tailNode)
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

            } else if (listSize < other.listSize)
            {
                equal = true;
                node_pointer left = headNode->next;
                node_pointer right = other.headNode->next;

                while (left != tailNode)
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

        [[nodiscard]] bool operator < (const SinglyList<TYPE, ALLOCATOR>& other)
        {
            bool equal;

            if (listSize == other.listSize)
            {
                equal = false;
                node_pointer left = headNode->next;
                node_pointer right = other.headNode->next;

                while (left != tailNode)
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
            } else if (listSize > other.listSize)
            {
                equal = false;
                node_pointer left = headNode->next;
                node_pointer right = other.headNode->next;

                while (right != other.tailNode)
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

            } else if (listSize < other.listSize)
            {
                equal = true;
                node_pointer left = headNode->next;
                node_pointer right = other.headNode->next;

                while (left != tailNode)
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
        size_type listSize;
        node_pointer headNode;
        node_pointer tailNode;

        inline void initList()
        {
            node_pointer workNode_1{nullptr};
            node_pointer workNode_2{nullptr};

            try
            {
                workNode_1 = allocator.allocate(1);
                workNode_2 = allocator.allocate(1);

                new(workNode_1) node_type{value_type(), nullptr};
                new(workNode_2) node_type{value_type(), nullptr};

            } catch (std::bad_alloc)
            {
                return;
            } catch (...)
            {
                (*workNode_1).~node_type();
                (*workNode_2).~node_type();

                allocator.deallocate(workNode_1, 1);
                allocator.deallocate(workNode_2, 1);
                return;
            }

            headNode = workNode_1;
            headNode->next = workNode_2;
            tailNode = workNode_2;
        }

        inline void splitList(node_pointer* head, node_pointer* first, node_pointer* second)
        {
            node_pointer slow = *head;
            node_pointer fast = slow->next;

            while (fast != tailNode)
            {
                fast = fast->next;
                if (fast != tailNode)
                {
                    fast = fast->next;
                    slow = slow->next;
                }
            }

            *first = *head;
            *second = slow->next;
            slow->next = tailNode;
        }

        inline void merge(node_pointer* result, node_pointer first, node_pointer second)
        {
            node_pointer currentNode = *result;
            node_pointer previousNode = nullptr;

            if(first != tailNode && second != tailNode)
            {
                if (first->data <= second->data)
                {
                    *result = first;
                    first = first->next;
                } else
                {
                    *result = second;
                    second = second->next;
                }
            }

            (*result)->next = tailNode;
            previousNode = *result;

            while (first != tailNode && second != tailNode)
            {
                if (first->data <= second->data)
                {
                    currentNode = first;
                    first = first->next;
                } else
                {
                    currentNode = second;
                    second = second->next;
                }
                currentNode->next = tailNode;
                previousNode->next = currentNode;
                previousNode = currentNode;
            }

            while (first != tailNode)
            {
                currentNode = first;
                first = first->next;
                currentNode->next = tailNode;
                previousNode->next = currentNode;
                previousNode = currentNode;
            }

            while (second != tailNode)
            {
                currentNode = second;
                second = second->next;
                currentNode->next = tailNode;
                previousNode->next = currentNode;
                previousNode = currentNode;
            }
        };

        inline void merge_sort(node_pointer* head)
        {
            if (*head == tailNode || (*head)->next == tailNode)
            {
                return;
            } else
            {
                node_pointer first;
                node_pointer second;
                splitList(head, &first, &second);

                merge_sort(&first);
                merge_sort(&second);

                merge(head, first, second);
            }
        }
    };
}

#endif //INC_3DGAME_SINGLY_LINKED_LIST_H
