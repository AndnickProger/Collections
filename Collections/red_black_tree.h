#ifndef INC_3DGAME_RED_BLACK_TREE_H
#define INC_3DGAME_RED_BLACK_TREE_H

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
    enum class RBTColor
    {
        RED,
        BLACK
    };

    template<typename TYPE>
    struct RedBlackTreeNode
    {
        using value_type                               = TYPE;
        using color_type                               = RBTColor;
        using reference                                = TYPE&;
        using const_reference                          = const TYPE&;
        using pointer                                  = TYPE*;
        using const_pointer                            = const TYPE*;
        using node_pointer                             = RedBlackTreeNode<TYPE>*;
        using const_node_pointer                       = const RedBlackTreeNode<TYPE>*;

        [[nodiscard]] inline bool operator == (const RedBlackTreeNode<TYPE>& other) const noexcept
        {
            return data == other.data && color == other.color && parent == other.parent &&
            left == other.left && right == other.right;
        }

        [[nodiscard]] inline bool operator != (const RedBlackTreeNode<TYPE>& other) const noexcept
        {
            return data != other.data || color != other.color || parent != other.parent ||
            left != other.left || right != other.right;
        }

        value_type data;
        color_type color;
        node_pointer parent;
        node_pointer left;
        node_pointer right;
    };

    template<typename TYPE>
    class RedBlackTreeIterator
    {
    public:

        using iterator_category                        = std::bidirectional_iterator_tag;
        using value_type                               = TYPE;
        using difference_type                          = std::int64_t;
        using reference                                = TYPE&;
        using pointer                                  = TYPE*;
        using iterator                                 = RedBlackTreeIterator<value_type>;
        using node_type                                = RedBlackTreeNode<value_type>;
        using node_pointer                             = RedBlackTreeNode<value_type>*;

        node_pointer node;

        RedBlackTreeIterator() noexcept :
        node{}
        {}

        RedBlackTreeIterator(node_pointer node_) noexcept :
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
            return node->data == other.node->data && node->color == other.node->color && node->parent == other.node->parent &&
            node->left == other.node->left && node->right == other.node->right;
        }

        [[nodiscard]] bool operator != (const iterator& other) const noexcept
        {
            return node->data != other.node->data || node->color != other.node->color || node->parent != other.node->parent ||
            node->left != other.node->left || node->right != other.node->right;
        }

    private:

        [[nodiscard]] inline node_pointer left_parent(node_pointer node)
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

        [[nodiscard]] inline node_pointer right_parent(node_pointer node)
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

        [[nodiscard]] inline node_pointer successor(node_pointer node)
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

        [[nodiscard]] inline node_pointer predecessor(node_pointer node)
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

        [[nodiscard]] inline node_pointer get_next(node_pointer node)
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

        [[nodiscard]] inline node_pointer get_previous(node_pointer node)
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
    class ConstRedBlackTreeIterator
    {
    public:

        using iterator_category                        = std::bidirectional_iterator_tag;
        using value_type                               = TYPE;
        using difference_type                          = std::int64_t;
        using reference                                = const TYPE&;
        using pointer                                  = const TYPE*;
        using iterator                                 = ConstRedBlackTreeIterator<value_type>;
        using node_type                                = RedBlackTreeNode<value_type>;
        using node_pointer                             = RedBlackTreeNode<value_type>*;

        node_pointer node;

        ConstRedBlackTreeIterator() noexcept :
        node{}
        {}

        ConstRedBlackTreeIterator(node_pointer node_) noexcept :
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
            return node->data == other.node->data && node->color == other.node->color && node->parent == other.node->parent &&
                   node->left == other.node->left && node->right == other.node->right;
        }

        [[nodiscard]] bool operator != (const iterator& other) const noexcept
        {
            return node->data != other.node->data || node->color != other.node->color || node->parent != other.node->parent ||
                   node->left != other.node->left || node->right != other.node->right;
        }

    private:

        [[nodiscard]] inline node_pointer left_parent(node_pointer node)
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

        [[nodiscard]] inline node_pointer right_parent(node_pointer node)
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

        [[nodiscard]] inline node_pointer successor(node_pointer node)
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

        [[nodiscard]] inline node_pointer predecessor(node_pointer node)
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

        [[nodiscard]] inline node_pointer get_next(node_pointer node)
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

        [[nodiscard]] inline node_pointer get_previous(node_pointer node)
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
    class RedBlackTree
    {
    public:

        using value_type                               = TYPE;
        using size_type                                = std::uint64_t;
        using difference_type                          = std::int64_t;
        using allocator_type                           = ALLOCATOR;
        using real_allocator_type                      = typename std::allocator_traits<ALLOCATOR>::template rebind_alloc<RedBlackTreeNode<TYPE>>;
        using reference                                = TYPE&;
        using const_reference                          = const TYPE&;
        using pointer                                  = TYPE*;
        using const_pointer                            = const TYPE*;
        using node_type                                = RedBlackTreeNode<value_type>;
        using node_pointer                             = RedBlackTreeNode<TYPE>*;
        using const_node_pointer                       = const RedBlackTreeNode<TYPE>*;
        using iterator                                 = RedBlackTreeIterator<value_type>;
        using const_iterator                           = ConstRedBlackTreeIterator<value_type>;
        using reverse_iterator	                       = std::reverse_iterator<iterator>;
        using const_reverse_iterator	               = std::reverse_iterator<const_iterator>;

        RedBlackTree(allocator_type allocator_ = ALLOCATOR()) :
        allocator(allocator_),
        treeSize{},
        rootNode{}, beginNode{}, endNode{}
        {
            initEndNode();
            rootNode = endNode;
            beginNode = endNode;
        }

        explicit RedBlackTree(const_reference value, allocator_type allocator_ = ALLOCATOR()) :
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

        RedBlackTree(std::initializer_list<value_type> list, allocator_type allocator_ = ALLOCATOR()) :
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
        RedBlackTree(const ITERATOR begin, const ITERATOR end, allocator_type allocator_ = ALLOCATOR()) :
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

        RedBlackTree(const RedBlackTree<TYPE, ALLOCATOR>& other)
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

        RedBlackTree<TYPE, ALLOCATOR>& operator = (const RedBlackTree<TYPE, ALLOCATOR>& other)
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

        RedBlackTree(RedBlackTree<TYPE, ALLOCATOR>&& other) noexcept :
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

        RedBlackTree<TYPE, ALLOCATOR>& operator = (RedBlackTree<TYPE, ALLOCATOR>&& other) noexcept
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

        virtual ~RedBlackTree()
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

        void push(const_reference value)
        {
            if (treeSize == 0)
            {
                if (endNode == nullptr)
                {
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
                    new(workNode) node_type{value_type(value), RBTColor::BLACK, nullptr, nullptr, endNode};
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
                    new(workNode) node_type{value_type(value), RBTColor::RED, nullptr, nullptr, nullptr};
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

                fix_insert(workNode);
            }
        }

        void push(value_type&& value)
        {
            if (treeSize == 0)
            {
                if (endNode == nullptr)
                {
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
                } catch (std::bad_alloc)
                {
                    return;
                }

                new(workNode) node_type{value_type(std::move(value)), RBTColor::BLACK, nullptr, nullptr, endNode};

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

                new(workNode) node_type{value_type(std::move(value)), RBTColor::RED, nullptr, nullptr, nullptr};

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

                fix_insert(workNode);
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
                    new(workNode) node_type{value_type(value), RBTColor::BLACK, nullptr, nullptr, endNode};
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
                    new(workNode) node_type{value_type(value), RBTColor::RED, nullptr, nullptr, nullptr};
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
                            fix_insert(workNode);
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
                            fix_insert(workNode);
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

                new(workNode) node_type{value_type(std::move(value)), RBTColor::BLACK, nullptr, nullptr, endNode};

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

                new(workNode) node_type{value_type(std::move(value)), RBTColor::RED, nullptr, nullptr, nullptr};

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
                            fix_insert(workNode);
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
                            fix_insert(workNode);
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
                    new(workNode) node_type{value_type(std::forward<Args>(args)...), RBTColor::BLACK, nullptr, nullptr, endNode};
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
                    new(workNode) node_type{value_type(std::forward<Args>(args)...), RBTColor::RED, nullptr, nullptr, nullptr};
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
                            fix_insert(workNode);
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
                            fix_insert(workNode);
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
                    node_pointer parentNode{nullptr};
                    node_pointer childNode{nullptr};
                    RBTColor removeColor;
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
                            return;
                        } else
                        {
                            parentNode = removeNode->parent;
                            removeColor = removeNode->color;
                            childNode = removeNode->right;
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

                                if (removeColor == RBTColor::BLACK)
                                {
                                    fix_delete(childNode, parentNode, true);
                                }
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
                                if (removeColor == RBTColor::BLACK)
                                {
                                    fix_delete(childNode, parentNode, false);
                                }
                            }
                        }
                    } else if ((removeNode->left != nullptr && removeNode->right == nullptr) ||
                               (removeNode->left != nullptr && removeNode->right == endNode))
                    {
                        parentNode = removeNode->parent;
                        removeColor = removeNode->color;
                        childNode = removeNode->left;
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
                            if (removeColor == RBTColor::BLACK)
                            {
                                fix_delete(childNode, parentNode, true);
                            }

                        } else
                        {
                            node_pointer leftChildNode = removeNode->left;
                            parentNode = removeNode->parent;

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

                            if (removeColor == RBTColor::BLACK)
                            {
                                fix_delete(childNode, parentNode, true);
                            }
                        }
                    } else if ((removeNode->left == nullptr && removeNode->right != nullptr) ||
                               (removeNode->left == nullptr && removeNode->right != endNode))
                    {
                        removeColor = removeNode->color;
                        parentNode = removeNode->parent;
                        childNode = removeNode->right;
                        if (removeNode == rootNode)
                        {
                            node_pointer rightChildNode = removeNode->right;
                            rightChildNode->parent = nullptr;

                            (*removeNode).~node_type();
                            allocator.deallocate(removeNode, 1);

                            rootNode = rightChildNode;
                            beginNode = rightChildNode;
                            --treeSize;

                            if (removeColor == RBTColor::BLACK)
                            {
                                fix_delete(childNode, parentNode, false);
                            }

                        } else
                        {
                            node_pointer rightChildNode = removeNode->right;
                            parentNode = removeNode->parent;

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

                            if (removeColor == RBTColor::BLACK)
                            {
                                fix_delete(childNode, parentNode, false);
                            }
                        }

                    } else if ((removeNode->left != nullptr && removeNode->right != nullptr) ||
                               (removeNode->left != nullptr && removeNode->right != endNode))
                    {
                        /*if (removeNode == rootNode)
                        {
                            node_pointer successorNode = removeNode->right;
                            while (successorNode->left != nullptr)
                            {
                                successorNode = successorNode->left;
                            }

                            childNode = successorNode;

                            removeNode->fileData = std::move(successorNode->fileData);

                            node_pointer successorParent = successorNode->parent;
                            node_pointer successorChild = successorNode->right;
                            removeColor = successorNode->color;
                            parentNode = successorParent;
                            childNode = successorChild;

                            bool isLeft;

                            if (successorParent->right == successorNode)
                            {
                                successorParent->right = successorChild;
                                isLeft = false;
                            } else
                            {
                                successorParent->left = successorChild;
                                isLeft = true;
                            }
                            successorChild->parent = successorParent;

                            (*successorNode).~node_type();
                            allocator.deallocate(successorNode, 1);

                            --treeSize;

                            if (removeColor == RBTColor::BLACK)
                            {
                                fix_delete(childNode, parentNode, isLeft);
                            }

                        } else
                        {
                            node_pointer successorNode = removeNode->right;
                            while (successorNode->left != nullptr)
                            {
                                successorNode = successorNode->left;
                            }

                            node_pointer successorParent = successorNode->parent;
                            node_pointer successorChild = successorNode->right;
                            removeColor = successorNode->color;
                            parentNode = successorParent;
                            childNode = successorChild;

                            removeNode->fileData = std::move(successorNode->fileData);

                            bool isLeft;
                            if (successorParent->right == successorNode)
                            {
                                successorParent->right = successorChild;
                                isLeft = false;
                            } else
                            {
                                successorParent->left = successorChild;
                                isLeft = true;
                            }
                            successorChild->parent = successorParent;

                            --treeSize;

                            (*successorNode).~node_type();
                            allocator.deallocate(successorNode, 1);

                            if (removeColor == RBTColor::BLACK)
                            {
                                fix_delete(childNode, parentNode, isLeft);
                            }
                        }*/

                        node_pointer successorNode = removeNode->right;
                        while (successorNode->left != nullptr)
                        {
                            successorNode = successorNode->left;
                        }

                        childNode = successorNode;

                        removeNode->data = std::move(successorNode->data);

                        node_pointer successorParent = successorNode->parent;
                        node_pointer successorChild = successorNode->right;
                        removeColor = successorNode->color;
                        parentNode = successorParent;
                        childNode = successorChild;

                        bool isLeft;

                        if (successorParent->right == successorNode)
                        {
                            successorParent->right = successorChild;
                            isLeft = false;
                        } else
                        {
                            successorParent->left = successorChild;
                            isLeft = true;
                        }
                        if (successorChild != nullptr)
                        {
                            successorChild->parent = successorParent;
                        }

                        (*successorNode).~node_type();
                        allocator.deallocate(successorNode, 1);

                        --treeSize;

                        if (removeColor == RBTColor::BLACK)
                        {
                            fix_delete(childNode, parentNode, isLeft);
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
                    node_pointer parentNode{nullptr};
                    node_pointer childNode{nullptr};
                    RBTColor removeColor;
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
                            return;
                        } else
                        {
                            parentNode = removeNode->parent;
                            removeColor = removeNode->color;
                            childNode = removeNode->right;
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

                                if (removeColor == RBTColor::BLACK)
                                {
                                    fix_delete(childNode, parentNode, true);
                                }
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
                                if (removeColor == RBTColor::BLACK)
                                {
                                    fix_delete(childNode, parentNode, false);
                                }
                            }
                        }
                    } else if ((removeNode->left != nullptr && removeNode->right == nullptr) ||
                               (removeNode->left != nullptr && removeNode->right == endNode))
                    {
                        parentNode = removeNode->parent;
                        removeColor = removeNode->color;
                        childNode = removeNode->left;
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

                            if (removeColor == RBTColor::BLACK)
                            {
                                fix_delete(childNode, parentNode, true);
                            }

                        } else
                        {
                            node_pointer leftChildNode = removeNode->left;
                            parentNode = removeNode->parent;

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

                            if (removeColor == RBTColor::BLACK)
                            {
                                fix_delete(childNode, parentNode, true);
                            }
                        }
                    } else if ((removeNode->left == nullptr && removeNode->right != nullptr) ||
                               (removeNode->left == nullptr && removeNode->right != endNode))
                    {
                        removeColor = removeNode->color;
                        parentNode = removeNode->parent;
                        childNode = removeNode->right;
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

                            if (removeColor == RBTColor::BLACK)
                            {
                                fix_delete(childNode, parentNode, false);
                            }

                        } else
                        {
                            node_pointer rightChildNode = removeNode->right;
                            parentNode = removeNode->parent;

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

                            if (removeColor == RBTColor::BLACK)
                            {
                                fix_delete(childNode, parentNode, false);
                            }
                        }

                    } else if ((removeNode->left != nullptr && removeNode->right != nullptr) ||
                               (removeNode->left != nullptr && removeNode->right != endNode))
                    {

                        node_pointer successorNode = removeNode->right;
                        while (successorNode->left != nullptr)
                        {
                            successorNode = successorNode->left;
                        }

                        childNode = successorNode;

                        removeNode->data = std::move(successorNode->data);

                        node_pointer successorParent = successorNode->parent;
                        node_pointer successorChild = successorNode->right;
                        removeColor = successorNode->color;
                        parentNode = successorParent;
                        childNode = successorChild;

                        bool isLeft;

                        if (successorParent->right == successorNode)
                        {
                            successorParent->right = successorChild;
                            isLeft = false;
                        } else
                        {
                            successorParent->left = successorChild;
                            isLeft = true;
                        }
                        if (successorChild != nullptr)
                        {
                            successorChild->parent = successorParent;
                        }

                        (*successorNode).~node_type();
                        allocator.deallocate(successorNode, 1);

                        --treeSize;

                        if (removeColor == RBTColor::BLACK)
                        {
                            fix_delete(childNode, parentNode, isLeft);
                        }
                    }
                }
            }
        }

        void clear()
        {
            clear_fun(rootNode);
            treeSize = 0;
            rootNode = endNode;
            beginNode = endNode;
            endNode->parent = nullptr;
        }

        void swap(RedBlackTree<TYPE, ALLOCATOR>& other)
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

        [[nodiscard]] bool operator == (const RedBlackTree<TYPE, ALLOCATOR>& other) const noexcept
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

        [[nodiscard]] bool operator != (const RedBlackTree<TYPE, ALLOCATOR>& other) const noexcept
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

        [[nodiscard]] bool operator >= (const RedBlackTree<TYPE, ALLOCATOR>& other) const noexcept
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

        [[nodiscard]] bool operator > (const RedBlackTree<TYPE, ALLOCATOR>& other) const noexcept
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

        [[nodiscard]] bool operator <= (const RedBlackTree<TYPE, ALLOCATOR>& other) const noexcept
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

        [[nodiscard]] bool operator < (const RedBlackTree<TYPE, ALLOCATOR>& other) const noexcept
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


    private:

        real_allocator_type allocator;
        size_type treeSize;
        node_pointer rootNode;
        node_pointer beginNode;
        node_pointer endNode;

        void print_tree(node_pointer node)
        {
            if (node != nullptr && node != endNode)
            {
                std::cout << "{" << node->data << ", ";
                if (node->color ==RBTColor::BLACK)
                {
                    std::cout << "black";
                } else
                {
                    std::cout << "red";
                }
                std::cout << " }, ";
                print_tree(node->left);
                print_tree(node->right);
            }
        }

        inline void initEndNode()
        {
            node_pointer workNode{nullptr};
            try
            {
                workNode = allocator.allocate(1);
                new(workNode) node_type{value_type(), RBTColor::BLACK, nullptr, nullptr, nullptr};
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

        inline void left_rotate(node_pointer node)
        {
            node_pointer pivot = node->right;
            node_pointer nodeParent = node->parent;
            node_pointer pivotLeft = pivot->left;

            pivot->parent = nodeParent;
            if (nodeParent == nullptr)
            {
                rootNode = pivot;
            } else
            {
                if (nodeParent->left == node)
                {
                    nodeParent->left = pivot;
                } else
                {
                    nodeParent->right = pivot;
                }
            }

            node->parent = pivot;
            pivot->left = node;
            node->right = pivotLeft;
            if (pivotLeft != nullptr)
            {
                pivotLeft->parent = node;
            }
        }

        inline void right_rotate(node_pointer node)
        {
            node_pointer pivot = node->left;
            node_pointer nodeParent = node->parent;
            node_pointer pivotRight = pivot->right;

            pivot->parent = nodeParent;
            if (nodeParent == nullptr)
            {
                rootNode = pivot;
            } else
            {
                if (nodeParent->left == node)
                {
                    nodeParent->left = pivot;
                } else
                {
                    nodeParent->right = pivot;
                }
            }
            node->parent = pivot;
            pivot->right = node;
            node->left = pivotRight;
            if (pivotRight != nullptr && pivotRight != endNode)
            {
                pivotRight->parent = node;
            }
        }

        inline node_pointer get_grandfather(node_pointer node)
        {
            if (node->parent != nullptr)
            {
                return node->parent->parent;
            } else
            {
                return nullptr;
            }
        }

        inline node_pointer get_uncle(node_pointer node)
        {
            node_pointer grandfather = get_grandfather(node);

            if (grandfather == nullptr)
            {
                return nullptr;
            }
            if (node->parent == grandfather->left)
            {
                return grandfather->right;
            } else
            {
                return grandfather->left;
            }
        }

        inline node_pointer get_brother(node_pointer node)
        {
            if (node->parent != nullptr)
            {
                if (node == node->parent->left)
                {
                    return node->parent->right;
                } else
                {
                    return node->parent->left;
                }
            } else
            {
                return nullptr;
            }
        }

        inline node_pointer get_brother_for_fix_delete(node_pointer node, node_pointer parent)
        {
            if (node != nullptr)
            {
                return get_brother(node);
            } else if ((parent->right == nullptr && parent->left == nullptr) ||
            (parent->left == nullptr && parent->right == endNode))
            {
                return nullptr;
            } else if ((parent->left != nullptr && parent->right == nullptr) ||
            (parent->left != nullptr && parent->right == endNode))
            {
                if (parent->left == node)
                {
                    return nullptr;
                } else
                {
                    return parent->left;
                }
            } else if ((parent->left == nullptr && parent->right != nullptr) ||
            (parent->left == nullptr && parent->right != endNode))
            {
                if (parent->right == node)
                {
                    return nullptr;
                } else
                {
                    return parent->right;
                }
            }

            return nullptr;
        }

        inline bool is_node_left_child(node_pointer node)
        {
            if (node->parent != nullptr)
            {
                node_pointer parent = node->parent;
                if (parent->left == node)
                {
                    return true;
                } else
                {
                    return false;
                }
            } else
            {
                return false;
            }
        }

        inline bool is_node_right_child(node_pointer node)
        {
            if (node->parent != nullptr)
            {
                node_pointer parent = node->parent;
                if (parent->left == node)
                {
                    return false;
                } else
                {
                    return true;
                }
            } else
            {
                return false;
            }
        }

        inline bool is_brother_childs_black(node_pointer brother)
        {
            RBTColor leftChildColor;
            RBTColor rightChildColor;

            if (brother != nullptr)
            {
                if (brother->left != nullptr)
                {
                    leftChildColor = brother->left->color;
                } else
                {
                    leftChildColor = RBTColor::BLACK;
                }

                if (brother->right != nullptr)
                {
                    rightChildColor = brother->right->color;
                } else
                {
                    rightChildColor = RBTColor::BLACK;
                }
            } else
            {
                return false;
            }

            return leftChildColor == RBTColor::BLACK && rightChildColor == RBTColor::BLACK;
        }

        inline bool is_brother_right_child_black(node_pointer brotherNode)
        {
            RBTColor rightChildColor = RBTColor::BLACK;

            if (brotherNode != nullptr)
            {
                if (brotherNode->right != nullptr)
                {
                    rightChildColor = brotherNode->right->color;
                } else
                {
                    return false;
                }
            } else
            {
                return false;
            }

            return rightChildColor == RBTColor::BLACK;
        }

        inline bool is_brother_left_child_black(node_pointer brotherNode)
        {
            RBTColor leftChildColor = RBTColor::BLACK;

            if (brotherNode != nullptr)
            {
                if (brotherNode->left != nullptr)
                {
                    leftChildColor = brotherNode->left->color;
                } else
                {
                    return false;
                }
            } else
            {
                return false;
            }

            return leftChildColor == RBTColor::BLACK;
        }

        void fix_insert(node_pointer node)
        {
            if (node == rootNode)
            {
                node->color = RBTColor::BLACK;
                return;
            } else
            {
                node_pointer nodeParent = node->parent;
                node_pointer nodeUncle{nullptr};
                node_pointer grandfatherNode{nullptr};

                while (nodeParent != nullptr && nodeParent->color == RBTColor::RED)
                {
                    nodeParent = node->parent;
                    nodeUncle = get_uncle(node);
                    grandfatherNode = get_grandfather(node);

                    if (is_node_left_child(nodeParent))
                    {
                        if (nodeUncle != nullptr && nodeUncle->color == RBTColor::RED)
                        {
                            nodeParent->color = RBTColor::BLACK;
                            nodeUncle->color = RBTColor::BLACK;
                            grandfatherNode->color = RBTColor::RED;
                            node = grandfatherNode;
                        } else
                        {
                            if (is_node_right_child(node))
                            {
                                node = nodeParent;
                                left_rotate(nodeParent);
                            }
                            nodeParent = node->parent;
                            grandfatherNode = get_grandfather(node);
                            nodeParent->color = RBTColor::BLACK;
                            grandfatherNode->color = RBTColor::RED;
                            right_rotate(grandfatherNode);
                        }
                    } else if (is_node_right_child(nodeParent))
                    {
                        if (nodeUncle != nullptr && nodeUncle->color == RBTColor::RED)
                        {
                            nodeParent->color = RBTColor::BLACK;
                            nodeUncle->color = RBTColor::BLACK;
                            grandfatherNode->color = RBTColor::RED;
                            node = grandfatherNode;
                        } else
                        {
                            if (is_node_left_child(node))
                            {
                                node = nodeParent;
                                right_rotate(node);
                            }
                            nodeParent = node->parent;
                            grandfatherNode = get_grandfather(node);
                            nodeParent->color = RBTColor::BLACK;
                            grandfatherNode->color = RBTColor::RED;
                            left_rotate(grandfatherNode);
                        }
                    }
                }
                rootNode->color = RBTColor::BLACK;
            }
        }

        void fix_delete(node_pointer childNode, node_pointer parentNode, bool leftChild)
        {
            node_pointer currentNode{nullptr};
            node_pointer brotherNode{nullptr};
            node_type nil{value_type(), RBTColor::BLACK, parentNode, nullptr, nullptr};
            bool isNilActivate = false;
            if (childNode == nullptr)
            {
                if (leftChild)
                {
                    parentNode->left = &nil;
                } else
                {
                    parentNode->right = &nil;
                }
                isNilActivate = true;
                currentNode = &nil;
            } else
            {
                currentNode = childNode;
            }

            while (currentNode != rootNode && currentNode->color == RBTColor::BLACK)
            {
                if (currentNode->parent->left == currentNode)
                {
                    brotherNode = currentNode->parent->right;
                    if (brotherNode != nullptr && brotherNode->color == RBTColor::RED)
                    {
                        brotherNode->color = RBTColor::BLACK;
                        currentNode->parent->color = RBTColor::RED;
                        left_rotate(currentNode->parent);
                        brotherNode = currentNode->parent->right;
                    }

                    if (is_brother_childs_black(brotherNode))
                    {
                        brotherNode->color = RBTColor::RED;
                        currentNode = currentNode->parent;
                    } else
                    {
                        if (is_brother_right_child_black(brotherNode))
                        {
                            if (brotherNode->left != nullptr)
                            {
                                brotherNode->left->color = RBTColor::BLACK;
                            }
                            brotherNode->color = RBTColor::RED;
                            right_rotate(brotherNode);
                            brotherNode = currentNode->parent->right;
                        }

                        if (brotherNode != nullptr)
                        {
                            brotherNode->color = currentNode->parent->color;
                            if (brotherNode->right != nullptr)
                            {
                                brotherNode->right->color = RBTColor::BLACK;
                            }
                        }
                        currentNode->parent->color = RBTColor::BLACK;
                        left_rotate(currentNode->parent);
                        currentNode = rootNode;
                    }
                } else
                {
                    brotherNode = currentNode->parent->left;
                    if (brotherNode != nullptr && brotherNode->color == RBTColor::RED)
                    {
                        brotherNode->color = RBTColor::BLACK;
                        currentNode->parent->color = RBTColor::RED;
                        right_rotate(currentNode->parent);
                        brotherNode = currentNode->parent->left;
                    }

                    if (is_brother_childs_black(brotherNode))
                    {
                        brotherNode->color = RBTColor::RED;
                        currentNode = currentNode->parent;
                    } else
                    {
                        if (is_brother_left_child_black(brotherNode))
                        {
                            if (brotherNode->right != nullptr)
                            {
                                brotherNode->right->color = RBTColor::BLACK;
                            }
                            brotherNode->color = RBTColor::RED;
                            left_rotate(brotherNode);
                            brotherNode = currentNode->parent->left;
                        }

                        if (brotherNode != nullptr)
                        {
                            brotherNode->color = currentNode->parent->color;
                            if (brotherNode->left != nullptr)
                            {
                                brotherNode->left->color = RBTColor::BLACK;
                            }
                        }
                        currentNode->parent->color = RBTColor::BLACK;
                        right_rotate(currentNode->parent);
                        currentNode = rootNode;
                    }
                }
            }

            currentNode->color = RBTColor::BLACK;

            if (isNilActivate)
            {
                node_pointer parent = nil.parent;
                if (parent->left == &nil)
                {
                    parent->left = nullptr;
                } else
                {
                    parent->right = nullptr;
                }
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
#endif //INC_3DGAME_RED_BLACK_TREE_H
