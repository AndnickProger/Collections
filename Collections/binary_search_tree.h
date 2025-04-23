#ifndef INC_3DGAME_BINARY_SEARCH_TREE_H
#define INC_3DGAME_BINARY_SEARCH_TREE_H

#include <stdexcept>
#include <cstdlib>
#include <initializer_list>
#include <cassert>
#include <iostream>
#include <iterator>
#include <utility>
#include <algorithm>

namespace bice
{
    template<typename TYPE>
    struct BinaryTreeNode
    {
        using value_type                               = TYPE;
        using reference                                = TYPE&;
        using const_reference                          = const TYPE&;
        using pointer                                  = TYPE*;
        using const_pointer                            = const TYPE*;
        using node_pointer                             = BinaryTreeNode<TYPE>*;
        using const_node_pointer                       = const BinaryTreeNode<TYPE>*;

        [[nodiscard]] inline bool operator == (const BinaryTreeNode<TYPE>& other) const noexcept
        {
            return data == other.data && parent == other.parent && left == other.left && right == other.right;
        }

        [[nodiscard]] inline bool operator != (const BinaryTreeNode<TYPE>& other) const noexcept
        {
            return data != other.data || parent != other.parent || left != other.left || right != other.right;
        }

        value_type data;
        node_pointer parent;
        node_pointer left;
        node_pointer right;
    };

    template<typename TYPE>
    class BinaryTreeIterator
    {
    public:

        using iterator_category                        = std::bidirectional_iterator_tag;
        using value_type                               = TYPE;
        using difference_type                          = std::int64_t;
        using reference                                = TYPE&;
        using pointer                                  = TYPE*;
        using iterator                                 = BinaryTreeIterator<value_type>;
        using node_type                                = BinaryTreeNode<value_type>;
        using node_pointer                             = BinaryTreeNode<value_type>*;

        node_pointer node;

        BinaryTreeIterator() noexcept :
        node{}
        {}

        BinaryTreeIterator(node_pointer node_) noexcept :
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
            node = get_next(node);
            return *this;
        }

        [[nodiscard]] iterator operator ++ (int) noexcept
        {
            iterator temp(*this);
            node = get_next(node);
            return temp;
        }

        [[nodiscard]] iterator& operator -- () noexcept
        {
            node = get_previous(node);
            return *this;
        }

        [[nodiscard]] iterator operator -- (int) noexcept
        {
            iterator temp(*this);
            node = get_previous(node);
            return temp;
        }

        [[nodiscard]] bool operator == (const iterator& other) const noexcept
        {
            return node->data == other.node->data && node->parent == other.node->parent &&
            node->left == other.node->left && node->right == other.node->right;
        }

        [[nodiscard]] bool operator != (const iterator& other) const noexcept
        {
            return node->data != other.node->data || node->parent != other.node->parent ||
            node->left != other.node->left || node->right != other.node->right;
        }

    private:

        inline node_pointer left_parent(node_pointer node)
        {
            node_pointer currentNode = node;
            node_pointer parentNode = node->parent;

            while (true)
            {
                if (parentNode == nullptr)
                {
                    break;
                } else if (parentNode->right == currentNode)
                {
                    currentNode = parentNode;
                    parentNode = parentNode->parent;
                    continue;
                } else if (parentNode->left == currentNode)
                {
                    break;
                }
            }

            return parentNode;
        }

        inline node_pointer right_parent(node_pointer node)
        {
            node_pointer currentNode = node;
            node_pointer parentNode = node->parent;

            while (true)
            {
                if (parentNode == nullptr)
                {
                    break;
                } else if (parentNode->left == currentNode)
                {
                    currentNode = parentNode;
                    parentNode = parentNode->parent;
                    continue;
                } else if (parentNode->right == currentNode)
                {
                    break;
                }
            }

            return parentNode;
        }

        inline node_pointer successor(node_pointer node)
        {
            if (node->right != nullptr)
            {
                node_pointer successorsNode = node->right;
                while (successorsNode->left != nullptr)
                {
                    successorsNode = successorsNode->left;
                }

                return successorsNode;

            } else
            {
                return nullptr;
            }
        }

        inline node_pointer predecessor(node_pointer node)
        {
            if (node->left != nullptr)
            {
                node_pointer predecessorNode = node->left;

                while (predecessorNode->right != nullptr)
                {
                    predecessorNode = predecessorNode->right;
                }

                return predecessorNode;
            } else
            {
                return nullptr;
            }
        }

        inline node_pointer get_next(node_pointer node)
        {
            if (node != nullptr)
            {
                node_pointer nextNode;
                if (node->right != nullptr )
                {
                    nextNode = successor(node);
                } else if (node->right == nullptr)
                {
                    nextNode = left_parent(node);
                }

                return nextNode;
            }

            return node;
        }

        inline node_pointer get_previous(node_pointer node)
        {
            if (node != nullptr)
            {
                node_pointer previousNode;
                if (node->left != nullptr)
                {
                    previousNode = predecessor(node);
                } else
                {
                    previousNode = right_parent(node);
                }

                return previousNode;
            }

            return node;
        }
    };

    template<typename TYPE>
    class ConstBinaryTreeIterator
    {
    public:

        using iterator_category                        = std::bidirectional_iterator_tag;
        using value_type                               = TYPE;
        using difference_type                          = std::int64_t;
        using reference                                = const TYPE&;
        using pointer                                  = const TYPE*;
        using iterator                                 = ConstBinaryTreeIterator<value_type>;
        using node_type                                = BinaryTreeNode<value_type>;
        using node_pointer                             = BinaryTreeNode<value_type>*;

        node_pointer node;

        ConstBinaryTreeIterator() noexcept :
        node{}
        {}

        ConstBinaryTreeIterator(node_pointer node_) noexcept :
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
            node = get_next(node);
            return *this;
        }

        [[nodiscard]] iterator operator ++ (int) noexcept
        {
            iterator temp(*this);
            node = get_next(node);
            return temp;
        }

        [[nodiscard]] iterator& operator -- () noexcept
        {
            node = get_previous(node);
            return *this;
        }

        [[nodiscard]] iterator operator -- (int) noexcept
        {
            iterator temp(*this);
            node = get_previous(node);
            return temp;
        }

        [[nodiscard]] bool operator == (const iterator& other) const noexcept
        {
            return node->data == other.node->data && node->parent == other.node->parent &&
            node->left == other.node->left && node->right == other.node->right;
        }

        [[nodiscard]] bool operator != (const iterator& other) const noexcept
        {
            return node->data != other.node->data || node->parent != other.node->parent ||
            node->left != other.node->left || node->right != other.node->right;
        }

    private:

        inline node_pointer left_parent(node_pointer node)
        {
            node_pointer currentNode = node;
            node_pointer parentNode = node->parent;

            while (true)
            {
                if (parentNode == nullptr)
                {
                    break;
                } else if (parentNode->right == currentNode)
                {
                    currentNode = parentNode;
                    parentNode = parentNode->parent;
                    continue;
                } else if (parentNode->left == currentNode)
                {
                    break;
                }
            }

            return parentNode;
        }

        inline node_pointer right_parent(node_pointer node)
        {
            node_pointer currentNode = node;
            node_pointer parentNode = node->parent;

            while (true)
            {
                if (parentNode == nullptr)
                {
                    break;
                } else if (parentNode->left == currentNode)
                {
                    currentNode = parentNode;
                    parentNode = parentNode->parent;
                    continue;
                } else if (parentNode->right == currentNode)
                {
                    break;
                }
            }

            return parentNode;
        }

        inline node_pointer successor(node_pointer node)
        {
            if (node->right != nullptr)
            {
                node_pointer successorsNode = node->right;
                while (successorsNode->left != nullptr)
                {
                    successorsNode = successorsNode->left;
                }

                return successorsNode;

            } else
            {
                return nullptr;
            }
        }

        inline node_pointer predecessor(node_pointer node)
        {
            if (node->left != nullptr)
            {
                node_pointer predecessorNode = node->left;

                while (predecessorNode->right != nullptr)
                {
                    predecessorNode = predecessorNode->right;
                }

                return predecessorNode;
            } else
            {
                return nullptr;
            }
        }

        inline node_pointer get_next(node_pointer node)
        {
            if (node != nullptr)
            {
                node_pointer nextNode;
                if (node->right != nullptr )
                {
                    nextNode = successor(node);
                } else if (node->right == nullptr)
                {
                    nextNode = left_parent(node);
                }

                return nextNode;
            }

            return node;
        }

        inline node_pointer get_previous(node_pointer node)
        {
            if (node != nullptr)
            {
                node_pointer previousNode;
                if (node->left != nullptr)
                {
                    previousNode = predecessor(node);
                } else
                {
                    previousNode = right_parent(node);
                }

                return previousNode;
            }

            return node;
        }

    };

    template<typename TYPE, typename ALLOCATOR = std::allocator<TYPE>>
    class BinarySearchTree
    {
    public:

        using value_type                               = TYPE;
        using size_type                                = std::uint64_t;
        using difference_type                          = std::int64_t;
        using allocator_type                           = ALLOCATOR;
        using real_allocator_type                      = typename std::allocator_traits<ALLOCATOR>::template rebind_alloc<BinaryTreeNode<TYPE>>;
        using reference                                = TYPE&;
        using const_reference                          = const TYPE&;
        using pointer                                  = TYPE*;
        using const_pointer                            = const TYPE*;
        using node_type                                = BinaryTreeNode<value_type>;
        using node_pointer                             = BinaryTreeNode<TYPE>*;
        using const_node_pointer                       = const BinaryTreeNode<TYPE>*;
        using iterator                                 = BinaryTreeIterator<value_type>;
        using const_iterator                           = ConstBinaryTreeIterator<value_type>;
        using reverse_iterator	                       = std::reverse_iterator<iterator>;
        using const_reverse_iterator	               = std::reverse_iterator<const_iterator>;

        BinarySearchTree(allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        treeSize{},
        rootNode{}, beginNode{}, endNode{}
        {
            initEndNode();
            rootNode = endNode;
            beginNode = endNode;
        }

        explicit BinarySearchTree(const_reference value, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        treeSize{},
        rootNode{}, beginNode{}, endNode{}
        {
            initEndNode();
            node_pointer workNode{nullptr};
            try
            {
                workNode = allocator.allocate(1);
                new(workNode) node_type{value_type(value), nullptr, nullptr, nullptr};
            } catch (std::bad_alloc)
            {
                return;
            } catch (...)
            {
                (*workNode).~node_type();
                allocator.deallocate(workNode, 1);
                return;
            }

            treeSize = 1;
            workNode->right = endNode;
            endNode->parent = workNode;
            rootNode = workNode;
            beginNode = workNode;
        }

        BinarySearchTree(std::initializer_list<value_type> list, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        treeSize{},
        rootNode{}, beginNode{}, endNode{}
        {
            initEndNode();
            if (endNode == nullptr)
            {
                throw std::out_of_range("Tree is not initialized");
            }
            for(auto iter = list.begin(); iter != list.end(); ++iter)
            {
                push(std::forward<const_reference>(*iter));
            }

            if (treeSize < list.size())
            {
                clear();
            }
        }

        template<typename ITERATOR>
        BinarySearchTree(const ITERATOR begin, const ITERATOR end, allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        treeSize{},
        rootNode{}, beginNode{}, endNode{}
        {
            initEndNode();
            if (endNode == nullptr)
            {
                throw std::out_of_range("Tree is not initialized");
            }

            difference_type distance{0};
            for(auto iter = begin; iter != end; ++iter)
            {
                ++distance;
            }
            if (distance > 0)
            {
                for(auto iter = begin; iter != end; ++iter)
                {
                    push(std::forward<const_reference>(*iter));
                }

                if (treeSize < distance)
                {
                    clear();
                }
            }
        }

        BinarySearchTree(const BinarySearchTree<TYPE, ALLOCATOR>& other)
        {
            initEndNode();
            rootNode = endNode;
            beginNode = endNode;
            treeSize = 0;
            if (other.treeSize > 0)
            {
                copy_fun(other.rootNode, other.endNode);

                if (treeSize < other.treeSize)
                {
                    clear();
                }
            }
        }

        BinarySearchTree<TYPE, ALLOCATOR>& operator = (const BinarySearchTree<TYPE, ALLOCATOR>& other)
        {
            clear_fun(rootNode);

            treeSize = 0;
            rootNode = endNode;
            beginNode = endNode;

            if (other.treeSize > 0)
            {
                copy_fun(other.rootNode, other.endNode);

                if (treeSize < other.treeSize)
                {
                    clear();
                }
            }

            return *this;
        }

        BinarySearchTree(BinarySearchTree<TYPE, ALLOCATOR>&& other) noexcept :
        treeSize(std::move(other.treeSize)),
        rootNode(std::move(other.rootNode)),
        beginNode(std::move(other.beginNode)),
        endNode(std::move(other.endNode))
        {
            other.treeSize = 0;
            other.rootNode = nullptr;
            other.beginNode = nullptr;
            other.endNode = nullptr;
        }

        BinarySearchTree<TYPE, ALLOCATOR>& operator = (BinarySearchTree<TYPE, ALLOCATOR>&& other) noexcept
        {
            clear_destructor(rootNode);

            treeSize = std::move(other.treeSize);
            rootNode = std::move(other.rootNode);
            beginNode = std::move(other.beginNode);
            endNode = std::move(other.endNode);

            other.treeSize = 0;
            other.rootNode = nullptr;
            other.beginNode = nullptr;
            other.endNode = nullptr;

            return *this;
        }

        virtual ~BinarySearchTree()
        {
            clear_destructor(rootNode);
        }

        [[nodiscard]] inline iterator begin() noexcept
        {
            return iterator(beginNode);
        }

        [[nodiscard]] inline const_iterator begin() const noexcept
        {
            return const_iterator(beginNode);
        }

        [[nodiscard]] inline const_iterator cbegin() const noexcept
        {
            return const_iterator(beginNode);
        }

        [[nodiscard]] inline iterator end() noexcept
        {
            return iterator(endNode);
        }

        [[nodiscard]] inline const_iterator end() const noexcept
        {
            return const_iterator(endNode);
        }

        [[nodiscard]] inline const_iterator cend() const noexcept
        {
            return const_iterator(endNode);
        }

        [[nodiscard]] inline reverse_iterator rbegin() noexcept
        {
            return reverse_iterator(endNode);
        }

        [[nodiscard]] inline const_reverse_iterator rbegin() const noexcept
        {
            return const_reverse_iterator(endNode);
        }

        [[nodiscard]] inline const_reverse_iterator crbegin() const noexcept
        {
            return const_reverse_iterator(endNode);
        }

        [[nodiscard]] inline reverse_iterator rend() noexcept
        {
            return reverse_iterator(beginNode);
        }

        [[nodiscard]] inline const_reverse_iterator rend() const noexcept
        {
            return const_reverse_iterator(beginNode);
        }

        [[nodiscard]] inline const_reverse_iterator crend() const noexcept
        {
            return const_reverse_iterator(beginNode);
        }

        [[nodiscard]] inline size_type size() const noexcept
        {
            return treeSize;
        }

        [[nodiscard]] inline size_type max_size() const noexcept
        {
            return std::numeric_limits<size_type>::max();
        }

        [[nodiscard]] inline bool empty() const noexcept
        {
            return treeSize == 0;
        }

        [[nodiscard]] inline allocator_type get_allocator() const noexcept
        {
            return allocator;
        }

        [[nodiscard]] inline reference front()
        {
            if (treeSize > 0)
            {
                return beginNode->data;
            } else
            {
                throw std::out_of_range("List is empty");
            }
        }

        [[nodiscard]] inline const_reference front() const
        {
            if (treeSize > 0)
            {
                return beginNode->data;
            } else
            {
                throw std::out_of_range("List is empty");
            }
        }

        [[nodiscard]] inline reference quick_front()
        {
            return beginNode->data;
        }

        [[nodiscard]] inline const_reference quick_front() const
        {
            return beginNode->data;
        }

        [[nodiscard]] inline reference back()
        {
            if (treeSize > 0)
            {
                return endNode->parent->data;
            } else
            {
                throw std::out_of_range("List is empty");
            }
        }

        [[nodiscard]] inline const_reference back() const
        {
            if (treeSize > 0)
            {
                return endNode->parent->data;
            } else
            {
                throw std::out_of_range("List is empty");
            }
        }

        [[nodiscard]] inline reference quick_back()
        {
            return endNode->parent->data;
        }

        [[nodiscard]] inline const_reference quick_back() const
        {
            return endNode->parent->data;
        }

        void clear()
        {
            clear_fun(rootNode);
            treeSize = 0;
            rootNode = endNode;
            beginNode = endNode;
            endNode->parent = nullptr;
        }

        void push(const_reference value)
        {
            if (treeSize == 0)
            {
                if (endNode == nullptr)
                {
                    beginNode = nullptr;
                    rootNode = nullptr;
                    initEndNode();
                    beginNode = endNode;
                    rootNode = endNode;
                }
                if (endNode == nullptr)
                {
                    throw std::out_of_range("Tree is not initialized");
                }

                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(value), nullptr, nullptr, endNode};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return;
                }

                treeSize = 1;
                rootNode = workNode;
                endNode->parent = workNode;
                beginNode = workNode;
            } else if (treeSize > 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(value), nullptr, nullptr, nullptr};
                } catch (std::bad_alloc)
                {
                    return;
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return;
                }
                node_pointer currentNode = rootNode;

                while (true)
                {
                    if (value < currentNode->data)
                    {
                        if (currentNode->left != nullptr)
                        {
                            currentNode = currentNode->left;
                            continue;
                        } else
                        {
                            workNode->parent = currentNode;
                            currentNode->left = workNode;
                            if (currentNode == beginNode)
                            {
                                beginNode = workNode;
                            }
                            ++treeSize;
                            break;
                        }
                    } else if (value > currentNode->data)
                    {
                        if (currentNode->right != nullptr && currentNode->right != endNode)
                        {
                            currentNode = currentNode->right;
                            continue;
                        } else
                        {
                            workNode->parent = currentNode;
                            if (currentNode->right == endNode)
                            {
                                workNode->right = endNode;
                                endNode->parent = workNode;
                            }
                            currentNode->right = workNode;
                            ++treeSize;
                            break;
                        }
                    }
                }
            }
        }

        void push(value_type&& value)
        {
            if (treeSize == 0)
            {
                if (endNode == nullptr)
                {
                    beginNode = nullptr;
                    rootNode = nullptr;
                    initEndNode();
                }
                if (endNode == nullptr)
                {
                    throw std::out_of_range("Tree is not initialized");
                }

                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    return;
                }

                new(workNode) node_type{value_type(std::move(value)), nullptr, nullptr, endNode};

                treeSize = 1;
                rootNode = workNode;
                endNode->parent = workNode;
                beginNode = workNode;
            } else if (treeSize > 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    return;
                }
                new(workNode) node_type{value_type(std::move(value)), nullptr, nullptr, nullptr};

                node_pointer currentNode = rootNode;

                while (true)
                {
                    if (value < currentNode->data)
                    {
                        if (currentNode->left != nullptr)
                        {
                            currentNode = currentNode->left;
                            continue;
                        } else
                        {
                            workNode->parent = currentNode;
                            currentNode->left = workNode;
                            if (currentNode == beginNode)
                            {
                                beginNode = workNode;
                            }
                            ++treeSize;
                            break;
                        }
                    } else if (value > currentNode->data)
                    {
                        if (currentNode->right != nullptr && currentNode->right != endNode)
                        {
                            currentNode = currentNode->right;
                            continue;
                        } else
                        {
                            workNode->parent = currentNode;
                            if (currentNode->right == endNode)
                            {
                                workNode->right = endNode;
                                endNode->parent = workNode;
                            }
                            currentNode->right = workNode;
                            ++treeSize;
                            break;
                        }
                    }
                }
            }
        }

        std::pair<iterator, bool> insert(const_reference value)
        {
            if (treeSize == 0)
            {
                if (endNode == nullptr)
                {
                    beginNode = nullptr;
                    rootNode = nullptr;
                    initEndNode();
                }
                if (endNode == nullptr)
                {
                    throw std::out_of_range("Tree is not initialized");
                }

                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(value), nullptr, nullptr, endNode};
                } catch (std::bad_alloc)
                {
                    return std::pair<iterator, bool>(iterator(endNode), false);
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return std::pair<iterator, bool>(iterator(endNode), false);
                }

                treeSize = 1;
                rootNode = workNode;
                endNode->parent = workNode;
                beginNode = workNode;
                return std::pair<iterator, bool>(iterator(workNode), true);
            } else if (treeSize > 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(value), nullptr, nullptr, nullptr};
                } catch (std::bad_alloc)
                {
                    return std::pair<iterator, bool>(iterator(endNode), false);
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return std::pair<iterator, bool>(iterator(endNode), false);
                }
                node_pointer currentNode = rootNode;

                while (true)
                {
                    if (value < currentNode->data)
                    {
                        if (currentNode->left != nullptr)
                        {
                            currentNode = currentNode->left;
                            continue;
                        } else
                        {
                            workNode->parent = currentNode;
                            currentNode->left = workNode;
                            if (currentNode == beginNode)
                            {
                                beginNode = workNode;
                            }
                            ++treeSize;
                            return std::pair<iterator, bool>(iterator(workNode), true);
                        }
                    } else if (value > currentNode->data)
                    {
                        if (currentNode->right != nullptr && currentNode->right != endNode)
                        {
                            currentNode = currentNode->right;
                            continue;
                        } else
                        {
                            workNode->parent = currentNode;
                            if (currentNode->right == endNode)
                            {
                                workNode->right = endNode;
                                endNode->parent = workNode;
                            }
                            currentNode->right = workNode;
                            ++treeSize;
                            return std::pair<iterator, bool>(iterator(workNode), true);
                        }
                    } else if (value == currentNode->data)
                    {
                        return std::pair<iterator, bool>(iterator(currentNode), true);
                    }
                }
            }

            return std::pair<iterator, bool>(iterator(endNode), false);
        }

        std::pair<iterator, bool> insert(value_type&& value)
        {
            if (treeSize == 0)
            {
                if (endNode == nullptr)
                {
                    beginNode = nullptr;
                    rootNode = nullptr;
                    initEndNode();
                }
                if (endNode == nullptr)
                {
                    throw std::out_of_range("Tree is not initialized");
                }

                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    return std::pair<iterator, bool>(iterator(endNode), false);
                }

                new(workNode) node_type{value_type(std::move(value)), nullptr, nullptr, endNode};

                treeSize = 1;
                rootNode = workNode;
                endNode->parent = workNode;
                beginNode = workNode;
                return std::pair<iterator, bool>(iterator(workNode), true);
            } else if (treeSize > 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                } catch (std::bad_alloc)
                {
                    return std::pair<iterator, bool>(iterator(endNode), false);
                }

                new(workNode) node_type{value_type(std::move(value)), nullptr, nullptr, nullptr};

                node_pointer currentNode = rootNode;

                while (true)
                {
                    if (value < currentNode->data)
                    {
                        if (currentNode->left != nullptr)
                        {
                            currentNode = currentNode->left;
                            continue;
                        } else
                        {
                            workNode->parent = currentNode;
                            currentNode->left = workNode;
                            if (currentNode == beginNode)
                            {
                                beginNode = workNode;
                            }
                            ++treeSize;
                            return std::pair<iterator, bool>(iterator(workNode), true);
                        }
                    } else if (value > currentNode->data)
                    {
                        if (currentNode->right != nullptr && currentNode->right != endNode)
                        {
                            currentNode = currentNode->right;
                            continue;
                        } else
                        {
                            workNode->parent = currentNode;
                            if (currentNode->right == endNode)
                            {
                                workNode->right = endNode;
                                endNode->parent = workNode;
                            }
                            currentNode->right = workNode;
                            ++treeSize;
                            return std::pair<iterator, bool>(iterator(workNode), true);
                        }
                    } else if (value == currentNode->data)
                    {
                        return std::pair<iterator, bool>(iterator(currentNode), true);
                    }
                }
            }

            return std::pair<iterator, bool>(iterator(endNode), false);
        }

        template<typename... Args >
        std::pair<iterator, bool> emplace(Args... args)
        {
            if (treeSize == 0)
            {
                if (endNode == nullptr)
                {
                    beginNode = nullptr;
                    rootNode = nullptr;
                    initEndNode();
                }
                if (endNode == nullptr)
                {
                    throw std::out_of_range("Tree is not initialized");
                }

                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(std::forward<Args>(args)...), nullptr, nullptr, endNode};
                } catch (std::bad_alloc)
                {
                    return std::pair<iterator, bool>(iterator(endNode), false);
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return std::pair<iterator, bool>(iterator(endNode), false);
                }

                treeSize = 1;
                rootNode = workNode;
                endNode->parent = workNode;
                beginNode = workNode;
                return std::pair<iterator, bool>(iterator(workNode), true);
            } else if (treeSize > 0)
            {
                node_pointer workNode{nullptr};
                try
                {
                    workNode = allocator.allocate(1);
                    new(workNode) node_type{value_type(std::forward<Args>(args)...), nullptr, nullptr, nullptr};
                } catch (std::bad_alloc)
                {
                    return std::pair<iterator, bool>(iterator(endNode), false);
                } catch (...)
                {
                    (*workNode).~node_type();
                    allocator.deallocate(workNode, 1);
                    return std::pair<iterator, bool>(iterator(endNode), false);
                }
                node_pointer currentNode = rootNode;

                while (true)
                {
                    if (workNode->data < currentNode->data)
                    {
                        if (currentNode->left != nullptr)
                        {
                            currentNode = currentNode->left;
                            continue;
                        } else
                        {
                            workNode->parent = currentNode;
                            currentNode->left = workNode;
                            if (currentNode == beginNode)
                            {
                                beginNode = workNode;
                            }
                            ++treeSize;
                            return std::pair<iterator, bool>(iterator(workNode), true);
                        }
                    } else if (workNode->data > currentNode->data)
                    {
                        if (currentNode->right != nullptr && currentNode->right != endNode)
                        {
                            currentNode = currentNode->right;
                            continue;
                        } else
                        {
                            workNode->parent = currentNode;
                            if (currentNode->right == endNode)
                            {
                                workNode->right = endNode;
                                endNode->parent = workNode;
                            }
                            currentNode->right = workNode;
                            ++treeSize;
                            return std::pair<iterator, bool>(iterator(workNode), true);
                        }
                    } else if (workNode->data == currentNode->data)
                    {
                        return std::pair<iterator, bool>(iterator(currentNode), true);
                    }
                }
            }

            return std::pair<iterator, bool>(iterator(endNode), false);
        }

        void remove(const_reference value)
        {
            if (treeSize > 0)
            {
                iterator findIter = find(value);
                node_pointer removeNode = findIter.node;
                if (removeNode != endNode)
                {
                    if ((removeNode->left == nullptr && removeNode->right == nullptr) ||
                            (removeNode->left == nullptr && removeNode->right == endNode))
                    {
                        if (rootNode == removeNode)
                        {
                            if (removeNode->right == endNode)
                            {
                                endNode->parent = nullptr;
                            }
                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);
                            rootNode = endNode;
                            beginNode = endNode;
                            endNode->parent = nullptr;
                            treeSize = 0;
                        } else
                        {
                            node_pointer parentNode =  removeNode->parent;
                            if (parentNode->left == removeNode)
                            {
                                parentNode->left = nullptr;
                                if (removeNode == beginNode)
                                {
                                    beginNode = parentNode;
                                }
                                (*removeNode).~node_type();
                                allocator.deallocate(removeNode, 1);
                                --treeSize;
                            } else
                            {
                                if (removeNode->right == endNode)
                                {
                                    endNode->parent = parentNode;
                                    parentNode->right = endNode;
                                } else
                                {
                                    parentNode->right = nullptr;
                                }

                                (*removeNode).~node_type();
                                allocator.deallocate(removeNode, 1);
                                --treeSize;
                            }
                        }
                    } else if ((removeNode->left != nullptr && removeNode->right == nullptr) ||
                    (removeNode->left != nullptr && removeNode->right == endNode))
                    {
                        if (rootNode == removeNode)
                        {
                            node_pointer leftChildNode = removeNode->left;
                            leftChildNode->parent = nullptr;
                            if (removeNode->right == endNode)
                            {
                                node_pointer leftRightNode = leftChildNode;
                                while (leftRightNode->right != nullptr)
                                {
                                    leftRightNode = leftRightNode->right;
                                }

                                leftRightNode->right = endNode;
                                endNode->parent = leftRightNode;
                            }

                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);

                            rootNode = leftChildNode;
                            --treeSize;

                            node_pointer minimumNode = rootNode;
                            while (minimumNode->left != nullptr)
                            {
                                minimumNode = minimumNode->left;
                            }
                            beginNode = minimumNode;

                        } else
                        {
                            node_pointer leftChildNode = removeNode->left;
                            node_pointer parentNode = removeNode->parent;

                            auto isNodeLeft = parentNode->left == removeNode;

                            if (removeNode->right == endNode)
                            {
                                node_pointer leftRightNode = leftChildNode;
                                while (leftRightNode->right != nullptr)
                                {
                                    leftRightNode = leftRightNode->right;
                                }

                                leftRightNode->right = endNode;
                                endNode->parent = leftRightNode;
                            }

                            if (isNodeLeft)
                            {
                                parentNode->left = leftChildNode;
                                leftChildNode->parent = parentNode;
                            } else
                            {
                                parentNode->right = leftChildNode;
                                leftChildNode->parent = parentNode;
                            }

                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);

                            --treeSize;

                            node_pointer minimumNode = rootNode;
                            while (minimumNode->left != nullptr)
                            {
                                minimumNode = minimumNode->left;
                            }
                            beginNode = minimumNode;
                        }
                    } else if ((removeNode->left == nullptr && removeNode->right != nullptr) ||
                    (removeNode->left == nullptr && removeNode->right != endNode))
                    {
                        if (removeNode == rootNode)
                        {
                            node_pointer rightChildNode = removeNode->right;
                            rightChildNode->parent = nullptr;

                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);

                            rootNode = rightChildNode;
                            beginNode = rightChildNode;
                            --treeSize;
                            node_pointer minimumNode = rootNode;
                            while (minimumNode->left != nullptr)
                            {
                                minimumNode = minimumNode->left;
                            }
                            beginNode = minimumNode;

                        } else
                        {
                            node_pointer rightChildNode = removeNode->right;
                            node_pointer parentNode = removeNode->parent;

                            if (removeNode == beginNode)
                            {
                                beginNode = rightChildNode;
                            }

                            auto isNodeLeft = parentNode->left == removeNode;

                            if (isNodeLeft)
                            {
                                parentNode->left = rightChildNode;
                                rightChildNode->parent = parentNode;
                            } else
                            {
                                parentNode->right = rightChildNode;
                                rightChildNode->parent = parentNode;
                            }

                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);

                            --treeSize;
                            node_pointer minimumNode = rootNode;
                            while (minimumNode->left != nullptr)
                            {
                                minimumNode = minimumNode->left;
                            }
                            beginNode = minimumNode;
                        }

                    } else if ((removeNode->left != nullptr && removeNode->right != nullptr) ||
                    (removeNode->left != nullptr && removeNode->right != endNode))
                    {
                        if (removeNode == rootNode)
                        {
                            node_pointer successorNode = removeNode->right;
                            while (successorNode->left != nullptr)
                            {
                                successorNode = successorNode->left;
                            }

                            node_pointer successorParent = successorNode->parent;
                            node_pointer removeLeft = removeNode->left;
                            node_pointer removeRight = removeNode->right;

                            successorParent->left = nullptr;
                            successorNode->parent = nullptr;

                            successorNode->left = removeLeft;
                            removeLeft->parent = successorNode;

                            successorNode->right = removeRight;
                            removeRight->parent = successorNode;

                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);

                            rootNode = successorNode;

                            --treeSize;

                        } else
                        {
                            node_pointer successorNode = removeNode->right;
                            while (successorNode->left != nullptr)
                            {
                                successorNode = successorNode->left;
                            }

                            node_pointer successorParent = successorNode->parent;
                            node_pointer removeParent = removeNode->parent;
                            node_pointer removeLeft = removeNode->left;
                            node_pointer removeRight = removeNode->right;

                            successorParent->left = nullptr;

                            if (removeParent->left == removeNode)
                            {
                                removeParent->left = successorNode;
                                successorNode->parent = removeParent;
                            } else
                            {
                                removeParent->right = successorNode;
                                successorNode->parent = removeParent;
                            }

                            successorNode->left = removeLeft;
                            removeLeft->parent = successorNode;

                            successorNode->right = removeRight;
                            removeRight->parent = successorNode;

                            --treeSize;

                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);
                        }
                    }
                }
            }
        }

        void remove(iterator element)
        {
            if (treeSize > 0)
            {
                node_pointer removeNode = element.node;
                if (removeNode != endNode)
                {
                    if ((removeNode->left == nullptr && removeNode->right == nullptr) ||
                        (removeNode->left == nullptr && removeNode->right == endNode))
                    {
                        if (rootNode == removeNode)
                        {
                            if (removeNode->right == endNode)
                            {
                                endNode->parent = nullptr;
                            }
                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);
                            rootNode = endNode;
                            beginNode = endNode;
                            endNode->parent = nullptr;
                            treeSize = 0;
                        } else
                        {
                            node_pointer parentNode = removeNode->parent;
                            if (parentNode->left == removeNode)
                            {
                                parentNode->left = nullptr;
                                if (removeNode == beginNode)
                                {
                                    beginNode = parentNode;
                                }
                                (*removeNode).~node_type();
                                allocator.deallocate(removeNode, 1);
                                --treeSize;
                            } else
                            {
                                if (removeNode->right == endNode)
                                {
                                    endNode->parent = parentNode;
                                    parentNode->right = endNode;
                                } else
                                {
                                    parentNode->right = nullptr;
                                }

                                (*removeNode).~node_type();
                                allocator.deallocate(removeNode, 1);
                                --treeSize;
                            }
                        }
                    } else if ((removeNode->left != nullptr && removeNode->right == nullptr) ||
                               (removeNode->left != nullptr && removeNode->right == endNode))
                    {
                        if (rootNode == removeNode)
                        {
                            node_pointer leftChildNode = removeNode->left;
                            leftChildNode->parent = nullptr;
                            if (removeNode->right == endNode)
                            {
                                node_pointer leftRightNode = leftChildNode;
                                while (leftRightNode->right != nullptr)
                                {
                                    leftRightNode = leftRightNode->right;
                                }

                                leftRightNode->right = endNode;
                                endNode->parent = leftRightNode;
                            }

                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);

                            rootNode = leftChildNode;
                            --treeSize;

                        } else
                        {
                            node_pointer leftChildNode = removeNode->left;
                            node_pointer parentNode = removeNode->parent;

                            auto isNodeLeft = parentNode->left == removeNode;

                            if (removeNode->right == endNode)
                            {
                                node_pointer leftRightNode = leftChildNode;
                                while (leftRightNode->right != nullptr)
                                {
                                    leftRightNode = leftRightNode->right;
                                }

                                leftRightNode->right = endNode;
                                endNode->parent = leftRightNode;
                            }

                            if (isNodeLeft)
                            {
                                parentNode->left = leftChildNode;
                                leftChildNode->parent = parentNode;
                            } else
                            {
                                parentNode->right = leftChildNode;
                                leftChildNode->parent = parentNode;
                            }

                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);

                            --treeSize;
                        }
                    } else if ((removeNode->left == nullptr && removeNode->right != nullptr) ||
                               (removeNode->left == nullptr && removeNode->right != endNode))
                    {
                        if (removeNode == rootNode)
                        {
                            node_pointer rightChildNode = removeNode->right;
                            rightChildNode->parent = nullptr;

                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);

                            rootNode = rightChildNode;
                            --treeSize;
                            node_pointer minimumNode = rootNode;
                            while (minimumNode->left != nullptr)
                            {
                                minimumNode = minimumNode->left;
                            }
                            beginNode = minimumNode;

                        } else
                        {
                            node_pointer rightChildNode = removeNode->right;
                            node_pointer parentNode = removeNode->parent;

                            if (removeNode == beginNode)
                            {
                                beginNode = rightChildNode;
                            }

                            auto isNodeLeft = parentNode->left == removeNode;

                            if (isNodeLeft)
                            {
                                parentNode->left = rightChildNode;
                                rightChildNode->parent = parentNode;
                            } else
                            {
                                parentNode->right = rightChildNode;
                                rightChildNode->parent = parentNode;
                            }

                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);

                            node_pointer minimumNode = rootNode;
                            while (minimumNode->left != nullptr)
                            {
                                minimumNode = minimumNode->left;
                            }
                            beginNode = minimumNode;

                            --treeSize;
                        }

                    } else if ((removeNode->left != nullptr && removeNode->right != nullptr) ||
                               (removeNode->left != nullptr && removeNode->right != endNode))
                    {
                        if (removeNode == rootNode)
                        {
                            node_pointer successorNode = removeNode->right;
                            while (successorNode->left != nullptr)
                            {
                                successorNode = successorNode->left;
                            }

                            node_pointer successorParent = successorNode->parent;
                            node_pointer removeLeft = removeNode->left;
                            node_pointer removeRight = removeNode->right;

                            successorParent->left = nullptr;
                            successorNode->parent = nullptr;

                            successorNode->left = removeLeft;
                            removeLeft->parent = successorNode;

                            successorNode->right = removeRight;
                            removeRight->parent = successorNode;

                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);

                            rootNode = successorNode;

                            --treeSize;

                        } else
                        {
                            node_pointer successorNode = removeNode->right;
                            while (successorNode->left != nullptr)
                            {
                                successorNode = successorNode->left;
                            }

                            node_pointer successorParent = successorNode->parent;
                            node_pointer removeParent = removeNode->parent;
                            node_pointer removeLeft = removeNode->left;
                            node_pointer removeRight = removeNode->right;

                            successorParent->left = nullptr;

                            if (removeParent->left == removeNode)
                            {
                                removeParent->left = successorNode;
                                successorNode->parent = removeParent;
                            } else
                            {
                                removeParent->right = successorNode;
                                successorNode->parent = removeParent;
                            }

                            successorNode->left = removeLeft;
                            removeLeft->parent = successorNode;

                            successorNode->right = removeRight;
                            removeRight->parent = successorNode;

                            --treeSize;

                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);
                        }
                    }
                }
            }
        }

        iterator find(const_reference value)
        {
            if (endNode == nullptr && treeSize == 0)
            {
                throw std::out_of_range("Tree is not initialized");
            } else if (treeSize == 0 && endNode != nullptr)
            {
                return iterator(endNode);
            } else
            {
                node_pointer currentNode = rootNode;
                node_pointer findNode{};

                while (true)
                {
                    if (value == currentNode->data)
                    {
                        findNode = currentNode;
                        break;
                    } else if (value > currentNode->data)
                    {
                        if (currentNode->right != nullptr && currentNode != endNode)
                        {
                            currentNode = currentNode->right;
                            continue;
                        } else
                        {
                            findNode = endNode;
                            break;
                        }
                    } else if (value < currentNode->data)
                    {
                        if (currentNode->left != nullptr)
                        {
                            currentNode = currentNode->left;
                            continue;
                        } else
                        {
                            findNode = endNode;
                            break;
                        }
                    }
                }

                return iterator(findNode);
            }
        }

        void swap(BinarySearchTree<TYPE, ALLOCATOR>& other)
        {
            const auto tempSize = treeSize;
            const auto tempRoot = rootNode;
            const auto tempBegin = beginNode;
            const auto tempEnd = endNode;

            treeSize = other.treeSize;
            rootNode = other.rootNode;
            beginNode = other.beginNode;
            endNode = other.endNode;

            other.treeSize = tempSize;
            other.rootNode = tempRoot;
            other.beginNode = tempBegin;
            other.endNode = tempEnd;
        }

        [[nodiscard]] bool operator == (const BinarySearchTree<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal = true;
            if (treeSize == other.treeSize)
            {
                const_iterator left = cbegin();
                const_iterator right = other.cbegin();

                while (left != cend())
                {
                    if (*left != *right)
                    {
                        equal = false;
                        break;
                    }
                    ++left;
                    ++right;
                }
            } else
            {
                equal = false;
            }
            return equal;
        }

        [[nodiscard]] bool operator != (const BinarySearchTree<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal = false;
            if (treeSize == other.treeSize)
            {
                const_iterator left = cbegin();
                const_iterator right = other.cbegin();

                while (left != cend())
                {
                    if (*left != *right)
                    {
                        equal = true;
                        break;
                    }
                    ++left;
                    ++right;
                }
            } else
            {
                equal = true;
            }
            return equal;
        }

        [[nodiscard]] bool operator >= (const BinarySearchTree<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (treeSize == other.treeSize)
            {
                equal = true;
                const_iterator left = cbegin();
                const_iterator right = other.cbegin();

                while (left != cend())
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
                    ++left;
                    ++right;
                }
            } else if (treeSize > other.treeSize)
            {
                equal = true;
                const_iterator left = cbegin();
                const_iterator right = other.cbegin();

                while (right != other.cend())
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
                    ++left;
                    ++right;
                }

            } else if (treeSize < other.treeSize)
            {
                equal = false;
                const_iterator left = cbegin();
                const_iterator right = other.cbegin();

                while (left != cend())
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
                    ++left;
                    ++right;
                }
            }

            return equal;
        }

        [[nodiscard]] bool operator > (const BinarySearchTree<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (treeSize == other.treeSize)
            {
                equal = false;
                const_iterator left = cbegin();
                const_iterator right = other.cbegin();

                while (left != cend())
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
                    ++left;
                    ++right;
                }
            } else if (treeSize > other.treeSize)
            {
                equal = true;
                const_iterator left = cbegin();
                const_iterator right = other.cbegin();

                while (right != other.cend())
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
                    ++left;
                    ++right;
                }
            } else if (treeSize < other.treeSize)
            {
                equal = false;
                const_iterator left = cbegin();
                const_iterator right = other.cbegin();

                while (left != cend())
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
                    ++left;
                    ++right;
                }
            }

            return equal;
        }

        [[nodiscard]] bool operator <= (const BinarySearchTree<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (treeSize == other.treeSize)
            {
                equal = true;
                const_iterator left = cbegin();
                const_iterator right = other.cbegin();

                while (left != cend())
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
                    ++left;
                    ++right;
                }

            } else if (treeSize > other.treeSize)
            {
                equal = false;
                const_iterator left = cbegin();
                const_iterator right = other.cbegin();

                while (right != other.cend())
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
                    ++left;
                    ++right;
                }

            } else if (treeSize < other.treeSize)
            {
                equal = true;
                const_iterator left = cbegin();
                const_iterator right = other.cbegin();

                while (left != cend())
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
                    ++left;
                    ++right;
                }
            }

            return equal;
        }

        [[nodiscard]] bool operator < (const BinarySearchTree<TYPE, ALLOCATOR>& other) const noexcept
        {
            bool equal;

            if (treeSize == other.treeSize)
            {
                equal = false;
                const_iterator left = cbegin();
                const_iterator right = other.cbegin();

                while (left != cend())
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
                    ++left;
                    ++right;
                }
            } else if (treeSize > other.treeSize)
            {
                equal = false;
                const_iterator left = cbegin();
                const_iterator right = other.cbegin();

                while (right != other.cend())
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
                    ++left;
                    ++right;
                }

            } else if (treeSize < other.treeSize)
            {
                equal = true;
                const_iterator left = cbegin();
                const_iterator right = other.cbegin();

                while (left != cend())
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
                    ++left;
                    ++right;
                }
            }

            return equal;
        }

        void print()
        {
            preorderTraversal_(rootNode);
            std::cout << "\n";
        }

    private:

        real_allocator_type allocator;
        size_type treeSize;
        node_pointer rootNode;
        node_pointer beginNode;
        node_pointer endNode;

        inline void initEndNode()
        {
            node_pointer workNode{nullptr};
            try
            {
                workNode = allocator.allocate(1);
                new(workNode) node_type{value_type(), nullptr, nullptr, nullptr};
            } catch (std::bad_alloc)
            {
                return;
            } catch (...)
            {
                (*workNode).~node_type();
                allocator.deallocate(workNode, 1);
                return;
            }

            endNode = workNode;
        }

        inline node_pointer getSuccessor(node_pointer node)
        {
            if (node->right != nullptr && node->right != endNode)
            {
                node_pointer successorsNode = node->right;
                while (successorsNode->left != nullptr)
                {
                    successorsNode = successorsNode->left;
                }

                return successorsNode;

            } else
            {
                return nullptr;
            }
        }

        inline node_pointer getPredecessor(node_pointer node)
        {
            if (node->left != nullptr)
            {
                node_pointer predecessorNode = node->left;

                while (predecessorNode->right != nullptr && predecessorNode->right != endNode)
                {
                    predecessorNode = predecessorNode->right;
                }

                return predecessorNode;
            } else
            {
                return nullptr;
            }
        }

        inline node_pointer getLeftParent(node_pointer node)
        {
            node_pointer currentNode = node;
            node_pointer parentNode = node->parent;

            while (true)
            {
                if (parentNode == nullptr)
                {
                    break;
                } else if (parentNode->right == currentNode)
                {
                    currentNode = parentNode;
                    parentNode = parentNode->parent;
                    continue;
                } else if (parentNode->left == currentNode)
                {
                    break;
                }
            }

            return parentNode;
        }

        inline node_pointer getRightParent(node_pointer node)
        {
            node_pointer currentNode = node;
            node_pointer parentNode = node->parent;

            while (true)
            {
                if (parentNode == nullptr)
                {
                    break;
                } else if (parentNode->left == currentNode)
                {
                    currentNode = parentNode;
                    parentNode = parentNode->parent;
                    continue;
                } else if (parentNode->right == currentNode)
                {
                    break;
                }
            }

            return parentNode;
        }

        inline node_pointer getNext(node_pointer node)
        {
            if (node != nullptr)
            {
                node_pointer nextNode;
                if (node->right != nullptr && node->right != endNode)
                {
                    nextNode = getSuccessor(node);
                } else if (node->right == nullptr || node->right == endNode)
                {
                    nextNode = getLeftParent(node);
                }

                return nextNode;
            } else
            {
                throw std::invalid_argument("Node cannot be nullptr");
            }
        }

        inline node_pointer getPrevious(node_pointer node)
        {
            if (node != nullptr)
            {
                node_pointer previousNode;
                if (node->left != nullptr)
                {
                    previousNode = getPredecessor(node);
                } else
                {
                    previousNode = getRightParent(node);
                }

                return previousNode;
            } else
            {
                throw std::invalid_argument("Node cannot be nullptr");
            }
        }

        void inorderTraversal_(node_pointer node)
        {
            if (node != nullptr && node != endNode)
            {
                inorderTraversal_(node->left);
                std::cout << node->data << ", ";
                inorderTraversal_(node->right);
            }
        }

        void preorderTraversal_(node_pointer node)
        {
            if (node != nullptr && node != endNode)
            {
                std::cout << node->data << ", ";
                preorderTraversal_(node->left);
                preorderTraversal_(node->right);
            }
        }

        void postorderTraversal_(node_pointer node)
        {
            if (node != nullptr && node != endNode)
            {
                postorderTraversal_(node->left);
                postorderTraversal_(node->right);
                std::cout << node->data << ", ";
            }
        }

        void clear_fun(node_pointer node)
        {
            if (node != nullptr && node != endNode)
            {
                clear_fun(node->left);
                clear_fun(node->right);
                (*node).~node_type();
                allocator.deallocate(node, 1);
            }
        }

        void clear_destructor(node_pointer node)
        {
            if (node != nullptr)
            {
                clear_destructor(node->left);
                clear_destructor(node->right);
                (*node).~node_type();
                allocator.deallocate(node, 1);
            }
        }

        void copy_fun(node_pointer node, node_pointer stop)
        {
            if (node != nullptr && node != stop)
            {
                push(std::forward<const_reference>(node->data));
                copy_fun(node->left, stop);
                copy_fun(node->right, stop);
            }
        }

    };


}

#endif //INC_3DGAME_BINARY_SEARCH_TREE_H
