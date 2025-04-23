#ifndef INC_3DGAME_DOUBLY_LINKED_LIST_H
#define INC_3DGAME_DOUBLY_LINKED_LIST_H

#include <stdexcept>
#include <cstdlib>
#include <initializer_list>
#include <cassert>
#include <iostream>
#include <iterator>

namespace bice
{
    template<typename TYPE>
    struct DoublyListNode
    {
        using value_type                               = TYPE;
        using reference                                = TYPE&;
        using const_reference                          = const TYPE&;
        using pointer                                  = TYPE*;
        using const_pointer                            = const TYPE*;
        using node_pointer                             = DoublyListNode<TYPE>*;
        using const_node_pointer                       = const DoublyListNode<TYPE>*;

        [[nodiscard]] inline bool operator == (const DoublyListNode<TYPE>& other) const noexcept
        {
            return data == other.data && previous == other.previous && next == other.next;
        }

        [[nodiscard]] inline bool operator != (const DoublyListNode<TYPE>& other) const noexcept
        {
            return data != other.data || previous != other.previous || next != other.next;
        }

        value_type data;
        node_pointer previous;
        node_pointer next;
    };

    template<typename TYPE>
    struct DoublyListIterator
    {
        using iterator_category                        = std::bidirectional_iterator_tag;
        using value_type                               = TYPE;
        using difference_type                          = std::int64_t;
        using reference                                = TYPE&;
        using pointer                                  = TYPE*;
        using iterator                                 = DoublyListIterator<value_type>;
        using node_type                                = DoublyListNode<value_type>;
        using node_pointer                             = DoublyListNode<value_type>*;

        node_pointer node;

        DoublyListIterator() noexcept :
        node{}
        {}

        DoublyListIterator(node_pointer node_) noexcept :
        node(node_)
        {}

        [[nodiscard]] inline reference operator * () noexcept
        {
            return node->data;
        }

        [[nodiscard]] inline pointer operator -> () noexcept
        {
            return static_cast<pointer>(&node->data);
        }

        [[nodiscard]] iterator& operator ++ () noexcept
        {
            node = node->next;
            return *this;
        }

        [[nodiscard]] iterator operator ++ (int) noexcept
        {
            iterator temp(*this);
            node = node->next;
            return temp;
        }

        [[nodiscard]] iterator& operator -- () noexcept
        {
            node = node->previous;
            return *this;
        }

        [[nodiscard]] iterator operator -- (int) noexcept
        {
            iterator temp(*this);
            node = node->previous;
            return temp;
        }

        [[nodiscard]] bool operator == (const iterator& other) const noexcept
        {
            return node->data == other.node->data && node->previous == other.node->previous && node->next == other.node->previous;
        }

        [[nodiscard]] bool operator != (const iterator& other) const noexcept
        {
            return node->data != other.node->data || node->previous != other.node->previous || node->next != other.node->next;
        }
    };

    template<typename TYPE>
    struct ConstDoublyListIterator
    {
        using iterator_category                        = std::bidirectional_iterator_tag;
        using value_type                               = TYPE;
        using difference_type                          = std::int64_t;
        using reference                                = const TYPE&;
        using pointer                                  = const TYPE*;
        using iterator                                 = ConstDoublyListIterator<value_type>;
        using node_type                                = DoublyListNode<value_type>;
        using node_pointer                             = DoublyListNode<value_type>*;

        node_pointer node;

        ConstDoublyListIterator() noexcept :
        node{}
        {}

        ConstDoublyListIterator(node_pointer node_) noexcept :
        node(node_)
        {}

        [[nodiscard]] inline reference operator * () const noexcept
        {
            return node->data;
        }

        [[nodiscard]] inline pointer operator -> () const noexcept
        {
            return static_cast<pointer>(&node->data);
        }

        [[nodiscard]] iterator& operator ++ () noexcept
        {
            node = node->next;
            return *this;
        }

        [[nodiscard]] iterator operator ++ (int) noexcept
        {
            iterator temp(*this);
            node = node->next;
            return temp;
        }

        [[nodiscard]] iterator& operator -- () noexcept
        {
            node = node->previous;
            return *this;
        }

        [[nodiscard]] iterator operator -- (int) noexcept
        {
            iterator temp(*this);
            node = node->previous;
            return temp;
        }

        [[nodiscard]] bool operator == (const iterator& other) const noexcept
        {
            return node->data == other.node->data && node->previous == other.node->previous && node->next == other.node->previous;
        }

        [[nodiscard]] bool operator != (const iterator& other) const noexcept
        {
            return node->data != other.node->data || node->previous != other.node->previous || node->next != other.node->next;
        }
    };

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    class DoublyList
    {
    public:

        using value_type                               = TYPE;
        using size_type                                = std::uint64_t;
        using difference_type                          = std::int64_t;
        using allocator_type                           = ALLOCATOR;
        using real_allocator_type                      = typename std::allocator_traits<ALLOCATOR>::template rebind_alloc<DoublyListNode<TYPE>>;
        using reference                                = TYPE&;
        using const_reference                          = const TYPE&;
        using pointer                                  = TYPE*;
        using const_pointer                            = const TYPE*;
        using node_type                                = DoublyListNode<value_type>;
        using node_pointer                             = DoublyListNode<TYPE>*;
        using const_node_pointer                       = const DoublyListNode<TYPE>*;
        using iterator                                 = DoublyListIterator<value_type>;
        using const_iterator                           = ConstDoublyListIterator<value_type>;
        using reverse_iterator	                       = std::reverse_iterator<iterator>;
        using const_reverse_iterator	               = std::reverse_iterator<const_iterator>;

        DoublyList(allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        listSize{},
        headNode{}, tailNode{}
        {
            initList();
        }

        explicit DoublyList(const size_type size, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        listSize{},
        headNode{}, tailNode{}
        {
            initList();

            if (size > 0)
            {
                node_pointer workNode{nullptr};
                node_pointer previousNode = nullptr;

                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(), nullptr, tailNode};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                headNode = workNode;
                previousNode = headNode;
                tailNode->previous = headNode;

                for(size_type idx = 1; idx < size; ++idx)
                {
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) node_type{value_type(), previousNode, tailNode};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = headNode;
                        node_pointer deleteNode = headNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(workNode, 1);
                            deleteNode = currentNode;
                        }

                        headNode = tailNode;
                        tailNode->previous = nullptr;
                        return;
                    } catch (...)
                    {
                        node_pointer currentNode = headNode;
                        node_pointer deleteNode = headNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(workNode, 1);
                            deleteNode = currentNode;
                        }

                        (*workNode).~node_type();
                        allocator.deallocate(workNode, 1);
                        listSize = 0;
                        headNode = tailNode;
                        tailNode->previous = nullptr;
                        return;
                    }

                    previousNode->next = workNode;
                    previousNode = workNode;
                }
                tailNode->previous = workNode;
                listSize = size;
            }
        }

        explicit DoublyList(const size_type size, const_reference value, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        listSize{},
        headNode{}, tailNode{}
        {
            initList();

            if (size > 0)
            {
                node_pointer workNode{nullptr};
                node_pointer previousNode = nullptr;

                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(value), nullptr, tailNode};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                headNode = workNode;
                previousNode = headNode;
                tailNode->previous = workNode;

                for(size_type idx = 1; idx < size; ++idx)
                {
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) node_type{value_type(value), previousNode, tailNode};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = headNode;
                        node_pointer deleteNode = headNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(workNode, 1);
                            deleteNode = currentNode;
                        }

                        headNode = tailNode;
                        tailNode->previous = nullptr;
                        return;
                    } catch (...)
                    {
                        node_pointer currentNode = headNode;
                        node_pointer deleteNode = headNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(workNode, 1);
                            deleteNode = currentNode;
                        }

                        (*workNode).~node_type();
                        allocator.deallocate(workNode, 1);
                        listSize = 0;
                        headNode = tailNode;
                        tailNode->previous = nullptr;
                        return;
                    }

                    previousNode->next = workNode;
                    previousNode = workNode;
                }
                tailNode->previous = workNode;
                listSize = size;
            }
        }

        DoublyList(std::initializer_list<value_type> list, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        listSize{},
        headNode{}, tailNode{}
        {
            initList();
            if (list.size() > 0)
            {
                node_pointer workNode{nullptr};
                node_pointer previousNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(*(list.begin())), nullptr, tailNode};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return;
                }

                headNode = workNode;
                previousNode = headNode;
                tailNode->previous = workNode;

                auto iter = list.begin();
                ++iter;
                while (iter != list.end())
                {
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) node_type{value_type(*iter), previousNode, tailNode};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = headNode;
                        node_pointer deleteNode = headNode;
                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(workNode, 1);
                            deleteNode = currentNode;
                        }
                        headNode = tailNode;
                        tailNode->previous = nullptr;
                        return;
                    } catch (...)
                    {
                        node_pointer currentNode = headNode;
                        node_pointer deleteNode = headNode;
                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(workNode, 1);
                            deleteNode = currentNode;
                        }
                        (*workNode).~node_type();
                        allocator.deallocate(workNode, 1);
                        headNode = tailNode;
                        tailNode->previous = nullptr;
                        return;
                    }
                    ++iter;
                    previousNode->next = workNode;
                    previousNode = workNode;
                }
                tailNode->previous = workNode;
                listSize = list.size();
            }
        }

        template<typename ITERATOR>
        DoublyList(const ITERATOR begin, const ITERATOR end, allocator_type allocator_ = ALLOCATOR())
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
                node_pointer previousNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(*begin), nullptr, tailNode};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return;
                }

                headNode = workNode;
                previousNode = headNode;
                tailNode->previous = workNode;

                auto iter = begin;
                ++iter;
                while (iter != end)
                {
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) node_type{value_type(*iter), previousNode, tailNode};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = headNode;
                        node_pointer deleteNode = headNode;
                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(workNode, 1);
                            deleteNode = currentNode;
                        }
                        headNode = tailNode;
                        return;
                    } catch (...)
                    {
                        node_pointer currentNode = headNode;
                        node_pointer deleteNode = headNode;
                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(workNode, 1);
                            deleteNode = currentNode;
                        }
                        (*workNode).~node_type();
                        allocator.deallocate(workNode, 1);
                        headNode = tailNode;
                        return;
                    }
                    ++iter;
                    previousNode->next = workNode;
                    previousNode = workNode;
                }
                tailNode->previous = workNode;
                listSize = distance;
            }
        }

        DoublyList(const DoublyList<TYPE, ALLOCATOR>& other)
        {
            initList();
            listSize = other.listSize;
            if (listSize > 0)
            {
                node_pointer otherNode = other.headNode;
                node_pointer workNode{nullptr};
                node_pointer previousNode = nullptr;

                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(otherNode->data), previousNode, tailNode};
                } catch (std::bad_alloc)
                {
                    listSize = 0;
                    return;
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    listSize = 0;
                    return;
                }

                tailNode->previous = workNode;
                headNode = workNode;
                previousNode = workNode;
                otherNode = otherNode->next;

                for(size_type idx = 1; idx < other.listSize; ++idx)
                {
                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) node_type{value_type(otherNode->data), previousNode, tailNode};
                    } catch (std::bad_alloc)
                    {
                        node_pointer currentNode = headNode;
                        node_pointer deleteNode = headNode;
                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        headNode = tailNode;
                        tailNode->previous = nullptr;
                        listSize = 0;
                        return;
                    } catch (...)
                    {
                        node_pointer currentNode = headNode;
                        node_pointer deleteNode = headNode;
                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        (*workNode).~node_type();
                        allocator.deallocate(workNode, 1);
                        headNode = tailNode;
                        tailNode->previous = nullptr;
                        listSize = 0;
                        return;
                    }
                    tailNode->previous = workNode;
                    previousNode->next = workNode;
                    previousNode = workNode;
                    otherNode = otherNode->next;
                }
                tailNode->previous = workNode;
            }
        }

        DoublyList<TYPE, ALLOCATOR>& operator = (const DoublyList<TYPE, ALLOCATOR>& other)
        {
            if (listSize == other.listSize && listSize > 0)
            {
                node_pointer otherNode = other.headNode;
                node_pointer currentNode = headNode;

                for(size_type idx = 0; idx < listSize; ++idx)
                {
                    try
                    {
                        currentNode->data = otherNode->data;
                    } catch (...)
                    {
                        node_pointer currentNode = headNode;
                        node_pointer deleteNode = headNode;

                        while (currentNode != tailNode)
                        {
                            currentNode = currentNode->next;
                            (*deleteNode).~node_type();
                            allocator.deallocate(deleteNode, 1);
                            deleteNode = currentNode;
                        }
                        tailNode->previous = nullptr;
                        headNode = tailNode;
                        listSize = 0;
                        return *this;
                    }
                }
            } else
            {
                if (headNode != nullptr)
                {
                    node_pointer currentNode = headNode;
                    node_pointer deleteNode = headNode;

                    while (currentNode != tailNode)
                    {
                        currentNode = currentNode->next;
                        (*deleteNode).~node_type();
                        allocator.deallocate(deleteNode, 1);
                        deleteNode = currentNode;
                    }
                    headNode = tailNode;
                    tailNode->previous = nullptr;
                    listSize = 0;
                }

                listSize = other.listSize;
                if (listSize > 0)
                {
                    node_pointer otherNode = other.headNode;
                    node_pointer workNode{nullptr};
                    node_pointer previousNode = nullptr;

                    try
                    {
                        workNode = allocator.allocate(1);
                        new(workNode) node_type{value_type(otherNode->data), previousNode, tailNode};
                    } catch (std::bad_alloc)
                    {
                        listSize = 0;
                        return *this;
                    } catch (...)
                    {
                        (*workNode).~node_type();
                        allocator.deallocate(workNode, 1);
                        listSize = 0;
                        return *this;
                    }

                    tailNode->previous = workNode;
                    headNode = workNode;
                    previousNode = workNode;
                    otherNode = otherNode->next;

                    for(size_type idx = 1; idx < other.listSize; ++idx)
                    {
                        try
                        {
                            workNode = allocator.allocate(1);
                            new(workNode) node_type{value_type(otherNode->data), previousNode, tailNode};
                        } catch (std::bad_alloc)
                        {
                            node_pointer currentNode = headNode;
                            node_pointer deleteNode = headNode;
                            while (currentNode != tailNode)
                            {
                                currentNode = currentNode->next;
                                (*deleteNode).~node_type();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }
                            headNode = tailNode;
                            tailNode->previous = nullptr;
                            listSize = 0;
                            return *this;
                        } catch (...)
                        {
                            node_pointer currentNode = headNode;
                            node_pointer deleteNode = headNode;
                            while (currentNode != tailNode)
                            {
                                currentNode = currentNode->next;
                                (*deleteNode).~node_type();
                                allocator.deallocate(deleteNode, 1);
                                deleteNode = currentNode;
                            }
                            (*workNode).~node_type();
                            allocator.deallocate(workNode, 1);
                            headNode = tailNode;
                            tailNode->previous = nullptr;
                            listSize = 0;
                            return *this;
                        }
                        tailNode->previous = workNode;
                        previousNode->next = workNode;
                        previousNode = workNode;
                        otherNode = otherNode->next;
                    }
                    tailNode->previous = workNode;
                }
            }
            return *this;
        }

        DoublyList(DoublyList<TYPE, ALLOCATOR>&& other) noexcept :
        listSize(std::move(other.listSize)),
        headNode(std::move(other.headNode)),
        tailNode(std::move(other.tailNode))
        {
            other.listSize = 0;
            other.headNode = nullptr;
            other.tailNode = nullptr;
        }

        DoublyList<TYPE, ALLOCATOR>& operator = (DoublyList<TYPE, ALLOCATOR>&& other) noexcept
        {
            if (headNode != nullptr && tailNode != nullptr)
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

            listSize = std::move(other.listSize);
            headNode = std::move(other.headNode);
            tailNode = std::move(other.tailNode);

            other.listSize = 0;
            other.headNode = nullptr;
            other.tailNode = nullptr;

            return *this;
        }

        virtual ~DoublyList()
        {
            if (headNode != nullptr && tailNode != nullptr)
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
        }

        [[nodiscard]] inline iterator begin() noexcept
        {
            return iterator(headNode);
        }

        [[nodiscard]] inline const_iterator begin() const noexcept
        {
            return const_iterator(headNode);
        }

        [[nodiscard]] inline const_iterator cbegin() const noexcept
        {
            return const_iterator(headNode);
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

        [[nodiscard]] inline reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(tailNode);
        }

        [[nodiscard]] inline const_reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(tailNode);
        }

        [[nodiscard]] inline const_reverse_iterator crbegin() const noexcept
        {
            return const_reverse_iterator(tailNode);
        }

        [[nodiscard]] inline reverse_iterator rend() noexcept
        {
            return reverse_iterator(headNode);
        }

        [[nodiscard]] inline const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(headNode);
        }

        [[nodiscard]] inline const_reverse_iterator crend() const noexcept
        {
            return const_reverse_iterator(headNode);
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

        [[nodiscard]] inline allocator_type get_allocator() const noexcept
        {
            return allocator;
        }

        [[nodiscard]] inline reference front()
        {
            if (listSize > 0)
            {
                return headNode->data;
            } else
            {
                throw std::out_of_range("List is empty");
            }
        }

        [[nodiscard]] inline const_reference front() const
        {
            if (listSize > 0)
            {
                return headNode->data;
            } else
            {
                throw std::out_of_range("List is empty");
            }
        }

        [[nodiscard]] inline reference quick_front()
        {
            return headNode->data;
        }

        [[nodiscard]] inline const_reference quick_front() const
        {
            return headNode->data;
        }

        [[nodiscard]] inline reference back()
        {
            if (listSize > 0)
            {
                return tailNode->previous->data;
            } else
            {
                throw std::out_of_range("List is empty");
            }
        }

        [[nodiscard]] inline const_reference back() const
        {
            if (listSize > 0)
            {
                return tailNode->previous->data;
            } else
            {
                throw std::out_of_range("List is empty");
            }
        }

        [[nodiscard]] inline reference quick_back()
        {
            return tailNode->previous->data;
        }

        [[nodiscard]] inline const_reference quick_back() const
        {
            return tailNode->previous->data;
        }

        void clear()
        {
            if (listSize > 0)
            {
                node_pointer currentNode = headNode;
                node_pointer deleteNode = headNode;

                while (currentNode != tailNode)
                {
                    currentNode = currentNode->next;
                    (*deleteNode).~node_type();
                    allocator.deallocate(deleteNode, 1);
                    deleteNode = currentNode;
                }
                tailNode->previous = nullptr;
                headNode = tailNode;
                listSize = 0;
            }
        }

        iterator insert(const_iterator position, const_reference value)
        {
            node_pointer previous = position.node->previous;
            node_pointer workNode{nullptr};
            try
            {
                workNode = allocator.allocate(1);
                new(workNode) node_type{value_type(value), nullptr, nullptr};
            } catch (std::bad_alloc)
            {
                return iterator(tailNode);
            } catch (...)
            {
                (*workNode).~node_type();
                allocator.deallocate(workNode, 1);
                return iterator(tailNode);
            }

            if(previous != nullptr)
            {
                previous->next = workNode;
                workNode->previous = previous;
                workNode->next = position.node;
                position.node->previous = workNode;
                ++listSize;
            } else
            {
                workNode->previous = nullptr;
                workNode->next = position.node;
                position.node->previous = workNode;
                headNode = workNode;
                ++listSize;
            }
            return iterator(workNode);
        }

        iterator insert(const_iterator position, value_type&& value)
        {
            node_pointer previous = position.node->previous;
            node_pointer workNode{nullptr};
            try
            {
                workNode = allocator.allocate(1);
            } catch (std::bad_alloc)
            {
                return iterator(tailNode);
            }

            new(workNode) node_type{value_type(std::move(value)), nullptr, nullptr};

            if(previous != nullptr)
            {
                previous->next = workNode;
                workNode->previous = previous;
                workNode->next = position.node;
                position.node->previous = workNode;
                ++listSize;
            } else
            {
                workNode->previous = nullptr;
                workNode->next = position.node;
                position.node->previous = workNode;
                headNode = workNode;
                ++listSize;
            }
            return iterator(workNode);
        }

        template<typename... Args>
        iterator emplace(const_iterator position, Args&&... args)
        {
            node_pointer previous = position.node->previous;
            node_pointer workNode{nullptr};
            try
            {
                workNode = allocator.allocate(1);
                new(workNode) node_type{value_type(std::forward<Args>(args)...), nullptr, nullptr};
            } catch (std::bad_alloc)
            {
                return iterator(tailNode);
            } catch (...)
            {
                (*workNode).~node_type();
                allocator.deallocate(workNode, 1);
                return iterator(tailNode);
            }

            if(previous != nullptr)
            {
                previous->next = workNode;
                workNode->previous = previous;
                workNode->next = position.node;
                position.node->previous = workNode;
                ++listSize;
            } else
            {
                workNode->previous = nullptr;
                workNode->next = position.node;
                position.node->previous = workNode;
                headNode = workNode;
                ++listSize;
            }
            return iterator(workNode);
        }

        iterator erase(const_iterator position)
        {
            if (listSize > 0 && position.node != tailNode)
            {
                node_pointer previous = position.node->previous;
                node_pointer next = position.node->next;

                --listSize;

                (*position.node).~node_type();
                allocator.deallocate(position.node, 1);
                if (previous == nullptr)
                {
                    next->previous = nullptr;
                    headNode = next;
                } else
                {
                    previous->next = next;
                    next->previous = previous;
                }
                return iterator(next);
            }
            return iterator(tailNode);
        }

        void push_front(const_reference value)
        {
            if (listSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(value), nullptr, tailNode};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return;
                }

                tailNode->previous = workNode;
                headNode = workNode;
                listSize = 1;
            } else
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(value), nullptr, headNode};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                headNode->previous = workNode;
                ++listSize;
                headNode = workNode;
            }
        }

        void push_front(value_type&& value)
        {
            if (listSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    return;
                }

                new(workNode) node_type{value_type(std::move(value)), nullptr, tailNode};

                tailNode->previous = workNode;
                headNode = workNode;
                listSize = 1;
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

                new(workNode) node_type{value_type(std::move(value)), nullptr, headNode};

                headNode->previous = workNode;
                ++listSize;
                headNode = workNode;
            }
        }

        template< class... Args >
        void emplace_front(Args&&... args)
        {
            if (listSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(std::forward<Args>(args)...), nullptr, tailNode};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return;
                }

                tailNode->previous = workNode;
                headNode = workNode;
                listSize = 1;
            } else
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(std::forward<Args>(args)...), nullptr, headNode};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                headNode->previous = workNode;
                ++listSize;
                headNode = workNode;
            }
        }

        void pop_front()
        {
            if (listSize == 1)
            {
                (*headNode).~node_type();
                allocator.deallocate(headNode, 1);
                listSize = 0;
                tailNode->previous = nullptr;
                headNode = tailNode;
            } else if (listSize > 1)
            {
                node_pointer next = headNode->next;
                (*headNode).~node_type();
                allocator.deallocate(headNode, 1);
                --listSize;
                next->previous = nullptr;
                headNode = next;
            }
        }

        void push_back(const_reference value)
        {
            if (listSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(value), nullptr, tailNode};
                } catch (std::bad_alloc)
                {
                    return;
                }catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                listSize = 1;
                tailNode->previous = workNode;
                headNode = workNode;
            } else
            {
                node_pointer workNode{nullptr};
                node_pointer previous = tailNode->previous;
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(value), previous, tailNode};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                previous->next = workNode;
                tailNode->previous = workNode;
                ++listSize;
            }
        }

        void push_back(value_type&& value)
        {
            if (listSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);

                } catch (std::bad_alloc)
                {
                    return;
                }
                new(workNode) node_type{value_type(std::move(value)), nullptr, tailNode};

                listSize = 1;
                tailNode->previous = workNode;
                headNode = workNode;
            } else
            {
                node_pointer workNode{nullptr};
                node_pointer previous = tailNode->previous;
                try
                {
                    workNode = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    return;
                }

                new(workNode) node_type{value_type(std::move(value)), previous, tailNode};

                previous->next = workNode;
                tailNode->previous = workNode;
                ++listSize;
            }
        }

        template< class... Args >
        void emplace_back(Args&&... args)
        {
            if (listSize == 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(std::forward<Args>(args)...), nullptr, tailNode};
                } catch (std::bad_alloc)
                {
                    return;
                }catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                listSize = 1;
                tailNode->previous = workNode;
                headNode = workNode;
            } else
            {
                node_pointer workNode{nullptr};
                node_pointer previous = tailNode->previous;
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(std::forward<Args>(args)...), previous, tailNode};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                previous->next = workNode;
                tailNode->previous = workNode;
                ++listSize;
            }
        }

        void pop_back()
        {
            if (listSize == 1)
            {
                (*headNode).~node_type();
                allocator.deallocate(headNode, 1);
                listSize = 0;
                tailNode->previous = nullptr;
                headNode = tailNode;
            } else if (listSize > 1)
            {
                node_pointer deleteNode = tailNode->previous;
                node_pointer previous = deleteNode->previous;
                (*deleteNode).~node_type();
                allocator.deallocate(deleteNode, 1);
                previous->next = tailNode;
                tailNode->previous = previous;
                --listSize;
            }
        }

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
            merge_sort(&headNode);
        }

        [[nodiscard]] bool operator == (const DoublyList<TYPE, ALLOCATOR>& other)
        {
            bool equal = true;
            if (listSize == other.listSize)
            {
                node_pointer left = headNode;
                node_pointer right = other.headNode;

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
                node_pointer left = headNode;
                node_pointer right = other.headNode;

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
                node_pointer left = headNode;
                node_pointer right = other.headNode;

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
                node_pointer left = headNode;
                node_pointer right = other.headNode;

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
                node_pointer left = headNode;
                node_pointer right = other.headNode;

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
                node_pointer left = headNode;
                node_pointer right = other.headNode;

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
                node_pointer left = headNode;
                node_pointer right = other.headNode;

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
                node_pointer left = headNode;
                node_pointer right = other.headNode;

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
                node_pointer left = headNode;
                node_pointer right = other.headNode;

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
                node_pointer left = headNode;
                node_pointer right = other.headNode;

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
                node_pointer left = headNode;
                node_pointer right = other.headNode;

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
                node_pointer left = headNode;
                node_pointer right = other.headNode;

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
                node_pointer left = headNode;
                node_pointer right = other.headNode;

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
                node_pointer left = headNode;
                node_pointer right = other.headNode;

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

        void initList()
        {
            node_pointer workNode{nullptr};
            try
            {
                workNode = allocator.allocate(1);
                new(workNode) node_type{value_type(), nullptr, nullptr};
            } catch (std::bad_alloc)
            {
                return;
            } catch (...)
            {
                (*workNode).~node_type();
                allocator.deallocate(workNode, 1);
                return;
            }
            tailNode = workNode;
            headNode = workNode;
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
            tailNode->previous = slow;
        }

        inline void merge(node_pointer* result, node_pointer first, node_pointer second)
        {
            node_pointer currentNode = *result;
            node_pointer previousNode = nullptr;

            if (first != tailNode && second != tailNode)
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
            tailNode->previous = *result;
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
                tailNode->previous = currentNode;
                previousNode->next = currentNode;
                currentNode->previous = previousNode;
                previousNode = currentNode;
            }

            while (first != tailNode)
            {
                currentNode = first;
                first = first->next;
                currentNode->next = tailNode;
                tailNode->previous = currentNode;
                previousNode->next = currentNode;
                currentNode->previous = previousNode;
                previousNode = currentNode;
            }

            while (second != tailNode)
            {
                currentNode = second;
                second = second->next;
                currentNode->next = tailNode;
                tailNode->previous = currentNode;
                previousNode->next = currentNode;
                currentNode->previous = previousNode;
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

#endif //INC_3DGAME_DOUBLY_LINKED_LIST_H
