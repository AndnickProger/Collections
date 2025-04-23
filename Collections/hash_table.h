#ifndef INC_3DGAME_HASH_TABLE_H
#define INC_3DGAME_HASH_TABLE_H

#include <stdexcept>
#include <cstdlib>
#include <initializer_list>
#include <cassert>
#include <iostream>
#include <iterator>
#include <utility>
#include <algorithm>
#include <functional>

namespace bice
{

    template<typename TYPE>
    struct HashNode
    {
        using value_type                               = TYPE;
        using reference                                = TYPE&;
        using const_reference                          = const TYPE&;
        using pointer                                  = TYPE*;
        using const_pointer                            = const TYPE*;
        using node_pointer                             = HashNode<TYPE>*;
        using const_node_pointer                       = const HashNode<TYPE>*;

        [[nodiscard]] inline bool operator == (const HashNode<value_type>& other) const noexcept
        {
            return data == other.data && previous == other.previous && next == other.next;
        }

        [[nodiscard]] inline bool operator != (const HashNode<value_type>& other) const noexcept
        {
            return data != other.data || previous != other.previous || next != other.next;
        }

        value_type data;
        node_pointer previous;
        node_pointer next;
    };

    template<typename HASH_TABLE>
    class HashTableIterator
    {
    public:

        using iterator_category                        = std::forward_iterator_tag;
        using key_type                                 = typename HASH_TABLE::key_type;
        using mapped_type                              = typename HASH_TABLE::mapped_type;
        using value_type                               = typename HASH_TABLE::value_type;
        using size_type                                = typename HASH_TABLE::size_type;
        using difference_type                          = typename HASH_TABLE::difference_type;
        using reference                                = typename HASH_TABLE::reference;
        using pointer                                  = typename HASH_TABLE::pointer;
        using iterator                                 = HashTableIterator<HASH_TABLE>;
        using node_type                                = HashNode<value_type>;
        using node_pointer                             = HashNode<value_type>*;
        using hash_table_type                          = HASH_TABLE;

        key_type first;
        mapped_type second;

        HashTableIterator() :
        index{},
        hashTable{}, node{},
        first{}, second{}
        {}

        HashTableIterator(
                const size_type& index_,
                hash_table_type* hashTable_,
                node_pointer node_
        ) :
                index(index_),
                hashTable(hashTable_),
                node(node_)
        {
            if (node != nullptr)
            {
                first = node->data.first;
                second = node->data.second;
            } else
            {
                first = {};
                second = {};
            }
        }

        [[nodiscard]] inline reference operator * () noexcept
        {
            return node->data;
        }

        [[nodiscard]] inline pointer operator -> () noexcept
        {
            return &(node->data);
        }

        iterator& operator ++ () noexcept
        {
            get_next();
            return *this;
        }

        iterator operator ++(int) noexcept
        {
            iterator temp(*this);
            get_next();
            return temp;
        }

        [[nodiscard]] bool operator == (const iterator& other) const noexcept
        {
            return index == other.index && hashTable == other.hashTable && node == other.node;
        }

        [[nodiscard]] bool operator != (const iterator& other) const noexcept
        {
            return index != other.index || hashTable != other.hashTable || node != other.node;
        }

        [[nodiscard]] inline size_type get_index() noexcept
        {
            return index;
        }

        [[nodiscard]] inline node_pointer* get_array() noexcept
        {
            return hashTable->get_array();
        }

        [[nodiscard]] inline node_pointer get_node() noexcept
        {
            return node;
        }

    private:

        size_type index;
        hash_table_type* hashTable;
        node_pointer node;

        void get_next()
        {
            const size_type arrayCapacity = hashTable->capacity();
            if (index <= arrayCapacity)
            {
                node_pointer nextNode{nullptr};
                if (node->next != nullptr)
                {
                    nextNode = node->next;
                } else
                {
                    size_type nextIndex = index + 1;
                    node_pointer* hashArray = hashTable->get_array();
                    while (nextIndex < arrayCapacity)
                    {
                        if (hashArray[nextIndex] != nullptr)
                        {
                            nextNode = hashArray[nextIndex];
                            break;
                        }
                        ++nextIndex;
                    }
                    if (nextNode == nullptr)
                    {
                        nextIndex = arrayCapacity;
                    }
                    index = nextIndex;
                }

                node = nextNode;
                if (node == nullptr)
                {
                    first = key_type();
                    second = mapped_type();
                } else
                {
                    first = node->data.first;
                    second = node->data.second;
                }
            }
        }

    };

    template<typename HASH_TABLE>
    class ConstHashTableIterator
    {
    public:

        using iterator_category                        = std::forward_iterator_tag;
        using key_type                                 = typename HASH_TABLE::key_type;
        using mapped_type                              = typename HASH_TABLE::mapped_type;
        using value_type                               = typename HASH_TABLE::value_type;
        using size_type                                = typename HASH_TABLE::size_type;
        using difference_type                          = typename HASH_TABLE::difference_type;
        using reference                                = typename HASH_TABLE::const_reference;
        using pointer                                  = typename HASH_TABLE::const_pointer;
        using iterator                                 = ConstHashTableIterator<HASH_TABLE>;
        using node_type                                = HashNode<value_type>;
        using node_pointer                             = HashNode<value_type>*;
        using hash_table_type                          = HASH_TABLE;

        key_type first;
        mapped_type second;

        ConstHashTableIterator() :
        index{},
        hashTable{}, node{},
        first{}, second{}
        {}

        ConstHashTableIterator(
                const size_type& index_,
                hash_table_type* hashTable_,
                node_pointer node_
        )  :
        index(index_),
        hashTable(hashTable_),
        node(node_)
        {
            if (node != nullptr)
            {
                first = node->data.first;
                second = node->data.second;
            } else
            {
                first = {};
                second = {};
            }
        }

        [[nodiscard]] inline reference operator * () const noexcept
        {
            return node->data;
        }

        [[nodiscard]] inline pointer operator -> () const noexcept
        {
            return &(node->data);
        }

        iterator& operator ++ () noexcept
        {
            get_next();
            return *this;
        }

        iterator operator ++ (int) noexcept
        {
            iterator temp(*this);
            get_next();
            return temp;
        }

        [[nodiscard]] bool operator == (const iterator& other) const noexcept
        {
            return index == other.index && hashTable == other.hashTable && node == other.node;
        }

        [[nodiscard]] bool operator != (const iterator& other) const noexcept
        {
            return index != other.index || hashTable != other.hashTable || node != other.node;
        }

        [[nodiscard]] inline size_type get_index() noexcept
        {
            return index;
        }

        [[nodiscard]] inline node_pointer* get_array() noexcept
        {
            return hashTable->get_array();
        }

        [[nodiscard]] inline node_pointer get_node() noexcept
        {
            return node;
        }

    private:

        size_type index;
        hash_table_type* hashTable;
        node_pointer node;

        void get_next()
        {
            const size_type arrayCapacity = hashTable->capacity();
            if (index <= arrayCapacity)
            {
                node_pointer nextNode{nullptr};
                if (node->next != nullptr)
                {
                    nextNode = node->next;
                } else
                {
                    size_type nextIndex = index + 1;
                    node_pointer* hashArray = hashTable->get_array();
                    while (nextIndex < arrayCapacity)
                    {
                        if (hashArray[nextIndex] != nullptr)
                        {
                            nextNode = hashArray[nextIndex];
                            break;
                        }
                        ++nextIndex;
                    }
                    if (nextNode == nullptr)
                    {
                        nextIndex = arrayCapacity;
                    }
                    index = nextIndex;
                }

                node = nextNode;
                if (node == nullptr)
                {
                    first = key_type();
                    second = mapped_type();
                } else
                {
                    first = node->data.first;
                    second = node->data.second;
                }
            }
        }
    };

    template<
            typename KEY,
            typename VALUE,
            typename HASH = std::hash<KEY>,
            typename KEY_EQUAL = std::equal_to<KEY>,
            typename ARRAY_ALLOCATOR = std::allocator<HashNode<std::pair<KEY, VALUE>>*>,
            typename LIST_ALLOCATOR = std::allocator<HashNode<std::pair<KEY, VALUE>>>
    >
    class HashTable
    {
    public:

        using key_type                                 = KEY;
        using mapped_type                              = VALUE;
        using value_type                               = std::pair<KEY, VALUE>;
        using size_type                                = std::uint64_t;
        using difference_type                          = std::int64_t;
        using hasher                                   = HASH;
        using key_equal                                = KEY_EQUAL;
        using array_allocator_type                     = ARRAY_ALLOCATOR;
        using list_allocator_type                      = LIST_ALLOCATOR;
        using reference                                = value_type&;
        using const_reference                          = const value_type&;
        using pointer                                  = value_type*;
        using const_pointer                            = const value_type*;
        using node_type                                = HashNode<value_type>;
        using node_pointer                             = node_type*;
        using array_type                               = node_type**;
        using const_array_type                         = const array_type;
        using iterator                                 = HashTableIterator<HashTable<KEY, VALUE, HASH, KEY_EQUAL, ARRAY_ALLOCATOR, LIST_ALLOCATOR>>;
        using const_iterator                           = ConstHashTableIterator<HashTable<KEY, VALUE, HASH, KEY_EQUAL, ARRAY_ALLOCATOR, LIST_ALLOCATOR>>;

        HashTable(
                const size_type count = 11,
                hasher hasher_ = HASH(),
                key_equal keyEqual_ = KEY_EQUAL(),
                array_allocator_type arrayAllocator_ = ARRAY_ALLOCATOR(),
                list_allocator_type listAllocator_ = LIST_ALLOCATOR()
                ) :
                arrayAllocator(arrayAllocator_),
                listAllocator(listAllocator_),
                hashFun(hasher_),
                keyEqual(keyEqual_),
                tableSize{}, arrayCapacity{}, array{}
                {
                    node_pointer* workArray{nullptr};
                    node_pointer* workIterator{nullptr};
                    try
                    {
                        workArray = arrayAllocator.allocate(count);
                    } catch (std::bad_alloc)
                    {
                        return;
                    }

                    workIterator = workArray;
                    for(size_type idx = 0; idx < count; ++idx)
                    {
                        *workIterator = nullptr;
                        ++workIterator;
                    }

                    arrayCapacity = count;
                    array = workArray;
                }

        HashTable(
                std::initializer_list<value_type> list,
                hasher hasher_ = HASH(),
                key_equal keyEqual_ = KEY_EQUAL(),
                array_allocator_type arrayAllocator_ = ARRAY_ALLOCATOR(),
                list_allocator_type listAllocator_ = LIST_ALLOCATOR()
                ) :
                arrayAllocator(arrayAllocator_),
                listAllocator(listAllocator_),
                hashFun(hasher_),
                keyEqual(keyEqual_),
                tableSize{}, arrayCapacity{}, array{}
                {
                    if (list.size() > 0)
                    {
                        for(auto iter = list.begin(); iter != list.end(); ++iter)
                        {
                            insert(*iter);
                        }

                        if (tableSize < list.size())
                        {
                            clear();
                        }
                    }
                }

        template<typename ITERATOR>
        HashTable(
                const ITERATOR begin, const ITERATOR end,
                hasher hasher_ = HASH(),
                key_equal keyEqual_ = KEY_EQUAL(),
                array_allocator_type arrayAllocator_ = ARRAY_ALLOCATOR(),
                list_allocator_type listAllocator_ = LIST_ALLOCATOR()
        )
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
                    insert(*iter);
                }
                if (tableSize < distance)
                {
                    clear();
                }
            }
        }

        HashTable(const HashTable& other)
        {
            arrayAllocator = other.arrayAllocator;
            listAllocator = other.listAllocator;
            hashFun = other.hashFun;
            keyEqual = other.keyEqual;
            tableSize = other.tableSize;
            arrayCapacity = other.arrayCapacity;
            array = nullptr;
            if (tableSize > 0)
            {
                node_pointer* workArray{nullptr};
                node_pointer* workIterator{nullptr};
                try
                {
                    workArray = arrayAllocator.allocate(arrayCapacity);
                } catch (std::bad_alloc)
                {
                    arrayCapacity = 0;
                    tableSize = 0;
                    return;
                }

                workIterator = workArray;

                for(size_type idx = 0; idx < arrayCapacity; ++idx)
                {
                    *workIterator = nullptr;
                    ++workIterator;
                }

                array = workArray;

                node_pointer otherArrayNode{nullptr};
                node_pointer arrayNode{nullptr};
                node_pointer workNode{nullptr};

                for(size_type idx = 0; idx < arrayCapacity; ++idx)
                {
                    otherArrayNode = other.array[idx];
                    arrayNode = array[idx];
                    while (otherArrayNode != nullptr)
                    {
                        try
                        {
                            workNode = listAllocator.allocate(1);
                            new(workNode) node_type{value_type(otherArrayNode->data), nullptr, nullptr};
                        } catch (std::bad_alloc)
                        {
                            clear();
                            return;
                        } catch (...)
                        {
                            (*workNode).~node_type();
                            listAllocator.deallocate(workNode, 1);
                            clear();
                            return;
                        }

                        if(array[idx] == nullptr)
                        {
                            array[idx] = workNode;
                            arrayNode = workNode;
                        } else
                        {
                            arrayNode->next = workNode;
                            workNode->previous = arrayNode;
                            arrayNode = workNode;
                        }

                        otherArrayNode = otherArrayNode->next;
                    }
                }
            }
        }

        HashTable& operator = (const HashTable& other)
        {
            if (arrayCapacity > 0 && array != nullptr)
            {
                if (tableSize > 0)
                {
                    node_pointer arrayNode{nullptr};
                    node_pointer deleteNode{nullptr};

                    for(size_type idx = 0; idx < arrayCapacity; ++idx)
                    {
                        arrayNode = array[idx];
                        deleteNode = arrayNode;
                        while (arrayNode != nullptr)
                        {
                            arrayNode = arrayNode->next;
                            (*deleteNode).~node_type();
                            listAllocator.deallocate(deleteNode, 1);
                            deleteNode = arrayNode;
                        }
                    }
                }
                arrayAllocator.deallocate(array, arrayCapacity);
            }

            array = nullptr;
            arrayCapacity = other.arrayCapacity;
            tableSize = other.tableSize;
            if (tableSize > 0)
            {
                node_pointer* workArray{nullptr};
                node_pointer* workIterator{nullptr};
                try
                {
                    workArray = arrayAllocator.allocate(arrayCapacity);
                } catch (std::bad_alloc)
                {
                    arrayCapacity = 0;
                    tableSize = 0;
                    return *this;
                }

                workIterator = workArray;

                for(size_type idx = 0; idx < arrayCapacity; ++idx)
                {
                    *workIterator = nullptr;
                    ++workIterator;
                }

                array = workArray;

                node_pointer otherArrayNode{nullptr};
                node_pointer arrayNode{nullptr};
                node_pointer workNode{nullptr};
                for(size_type idx = 0; idx < arrayCapacity; ++idx)
                {
                    otherArrayNode = other.array[idx];
                    arrayNode = array[idx];
                    while (otherArrayNode != nullptr)
                    {
                        try
                        {
                            workNode = listAllocator.allocate(1);
                            new(workNode) node_type{value_type(otherArrayNode->data), nullptr, nullptr};
                        } catch (std::bad_alloc)
                        {
                            clear();
                            return *this;
                        } catch (...)
                        {
                            (*workNode).~node_type();
                            listAllocator.deallocate(workNode, 1);
                            clear();
                            return *this;
                        }

                        if(array[idx] == arrayNode)
                        {
                            array[idx] = workNode;
                            arrayNode = workNode;
                        } else
                        {
                            arrayNode->next = workNode;
                            workNode->previous = arrayNode;
                            arrayNode = workNode;
                        }

                        otherArrayNode = otherArrayNode->next;
                    }
                }
            }

            return *this;
        }

        HashTable(HashTable&& other) noexcept :
        arrayAllocator(std::move(other.arrayAllocator)),
        listAllocator(std::move(other.listAllocator)),
        hashFun(std::move(other.hashFun)),
        keyEqual(std::move(other.keyEqual)),
        tableSize(std::move(other.tableSize)),
        arrayCapacity(std::move(other.arrayCapacity)),
        array(std::move(other.array))
        {
            other.tableSize = 0;
            other.arrayCapacity = 0;
            other.array = nullptr;
        }

        HashTable& operator = (HashTable&& other) noexcept
        {
            arrayAllocator = std::move(other.arrayAllocator);
            listAllocator = std::move(other.listAllocator);
            hashFun = std::move(other.hashFun);
            keyEqual = std::move(other.keyEqual);
            tableSize = std::move(other.tableSize);
            arrayCapacity = std::move(other.arrayCapacity);
            array = std::move(other.array);

            other.tableSize = 0;
            other.arrayCapacity = 0;
            other.array = nullptr;

            return *this;
        }

        virtual ~HashTable()
        {
            if (arrayCapacity > 0 && array != nullptr)
            {
                if (tableSize > 0)
                {
                    node_pointer arrayNode{nullptr};
                    node_pointer deleteNode{nullptr};

                    for(size_type idx = 0; idx < arrayCapacity; ++idx)
                    {
                        arrayNode = array[idx];
                        deleteNode = arrayNode;
                        while (arrayNode != nullptr)
                        {
                            arrayNode = arrayNode->next;
                            (*deleteNode).~node_type();
                            listAllocator.deallocate(deleteNode, 1);
                            deleteNode = arrayNode;
                        }
                    }
                }
                arrayAllocator.deallocate(array, arrayCapacity);
            }
        }

        [[nodiscard]] inline iterator begin() noexcept
        {
            size_type index = 0;
            node_pointer arrayNode = array[0];
            while (arrayNode == nullptr)
            {
                ++index;
                arrayNode = array[index];
            }
            return iterator(index, const_cast<HashTable*>(this), array[index]);
        }

        [[nodiscard]] inline const_iterator begin() const noexcept
        {
            size_type index = 0;
            node_pointer arrayNode = array[0];
            while (arrayNode == nullptr)
            {
                ++index;
                arrayNode = array[index];
            }
            return const_iterator(index, const_cast<HashTable*>(this), array[index]);
        }

        [[nodiscard]] inline const_iterator cbegin() const noexcept
        {
            size_type index = 0;
            node_pointer arrayNode = array[0];
            while (arrayNode == nullptr)
            {
                ++index;
                arrayNode = array[index];
            }
            return const_iterator(index, const_cast<HashTable*>(this), array[index]);
        }

        [[nodiscard]] inline iterator end() noexcept
        {
            return iterator(arrayCapacity, const_cast<HashTable*>(this), nullptr);
        }

        [[nodiscard]] inline const_iterator end() const noexcept
        {
            return const_iterator(arrayCapacity, const_cast<HashTable*>(this), nullptr);
        }

        [[nodiscard]] inline const_iterator cend() const noexcept
        {
            return const_iterator(arrayCapacity, const_cast<HashTable*>(this), nullptr);
        }

        [[nodiscard]] inline size_type size() const noexcept
        {
            return tableSize;
        }

        [[nodiscard]] inline size_type capacity() const noexcept
        {
            return arrayCapacity;
        }

        [[nodiscard]] inline size_type max_size() const noexcept
        {
            return std::numeric_limits<size_type>::max();
        }

        [[nodiscard]] inline float load_factor() const noexcept
        {
            auto n = (float)tableSize;
            auto m = (float)arrayCapacity;

            return n / m;
        }

        [[nodiscard]] inline bool empty() const noexcept
        {
            return tableSize == 0;
        }

        [[nodiscard]] inline array_type get_array() noexcept
        {
            return array;
        }

        [[nodiscard]] inline const_array_type get_array() const noexcept
        {
            return array;
        }

        void reserve(const size_type& newCapacity)
        {
            if (newCapacity > 0 && newCapacity > arrayCapacity)
            {
                node_pointer* workArray{nullptr};
                node_pointer* workIterator{nullptr};
                try
                {
                    workArray = arrayAllocator.allocate(newCapacity);
                } catch (std::bad_alloc)
                {
                    return;
                }

                workIterator = workArray;

                for(size_type idx = 0; idx < newCapacity; ++idx)
                {
                    *workIterator = nullptr;
                    ++workIterator;
                }

                node_pointer arrayNode{nullptr};
                node_pointer workNode{nullptr};
                node_pointer insertNode{nullptr};

                for(size_type idx = 0; idx < arrayCapacity; ++idx)
                {
                    arrayNode = array[idx];
                    while (arrayNode != nullptr)
                    {
                        insertNode = arrayNode;
                        auto key = arrayNode->data.first;
                        auto hashValue = (size_type)hashFun(key) % newCapacity;
                        workNode = workArray[hashValue];

                        if (workNode == nullptr)
                        {
                            workNode = insertNode;
                            insertNode->previous = nullptr;
                            insertNode->next = nullptr;
                        } else
                        {
                            while (workNode->next != nullptr)
                            {
                                workNode = workNode->next;
                            }
                            workNode->next = insertNode;
                            insertNode->previous = workNode;
                            insertNode->next = nullptr;
                        }

                        arrayNode = arrayNode->next;
                    }
                }

                arrayAllocator.deallocate(array, arrayCapacity);
                array = workArray;
                arrayCapacity = newCapacity;
            }
        }

        void insert(const_reference value)
        {
            if (arrayCapacity == 0 && array == nullptr)
            {
                node_pointer* workArray{nullptr};
                node_pointer* workIterator{nullptr};
                const size_type count = 11;
                try
                {
                    workArray = arrayAllocator.allocate(count);
                } catch (std::bad_alloc)
                {
                    return;
                }

                workIterator = workArray;
                for(size_type idx = 0; idx < count; ++idx)
                {
                    *workIterator = nullptr;
                    ++workIterator;
                }

                arrayCapacity = count;
                array = workArray;
            }

            auto loadFactor = load_factor();
            if (loadFactor >= 3.0f)
            {
                reserve(arrayCapacity * 2);
            }

            node_pointer workNode{nullptr};
            node_pointer arrayNode{nullptr};
            try
            {
                workNode = listAllocator.allocate(1);
                new(workNode) node_type{value_type(value), nullptr, nullptr};
            } catch (std::bad_alloc)
            {
                return;
            } catch (...)
            {
                (*workNode).~node_type();
                listAllocator.deallocate(workNode, 1);
                return;
            }

            auto key = workNode->data.first;
            auto hashValue = (size_type)hashFun(key) % arrayCapacity;
            arrayNode = array[hashValue];
            if (arrayNode == nullptr)
            {
                array[hashValue] = workNode;
            } else
            {
                while (arrayNode->next != nullptr)
                {
                    arrayNode = arrayNode->next;
                }

                arrayNode->next = workNode;
                workNode->previous = arrayNode;
            }

            ++tableSize;
        }

        void insert(value_type&& value)
        {
            if (arrayCapacity == 0 && array == nullptr)
            {
                node_pointer* workArray{nullptr};
                node_pointer* workIterator{nullptr};
                const size_type count = 11;
                try
                {
                    workArray = arrayAllocator.allocate(count);
                } catch (std::bad_alloc)
                {
                    return;
                }

                workIterator = workArray;
                for(size_type idx = 0; idx < count; ++idx)
                {
                    *workIterator = nullptr;
                    ++workIterator;
                }

                arrayCapacity = count;
                array = workArray;
            }

            auto loadFactor = load_factor();
            if (loadFactor >= 3.0f)
            {
                reserve(arrayCapacity * 2);
            }

            node_pointer workNode{nullptr};
            node_pointer arrayNode{nullptr};
            try
            {
                workNode = listAllocator.allocate(1);
            } catch (std::bad_alloc)
            {
                return;
            }

            new(workNode) node_type{value_type(std::move(value)), nullptr, nullptr};

            auto key = workNode->data.first;
            auto hashValue = (size_type)hashFun(key) % arrayCapacity;
            arrayNode = array[hashValue];
            if (arrayNode == nullptr)
            {
                array[hashValue] = workNode;
            } else
            {
                while (arrayNode->next != nullptr)
                {
                    arrayNode = arrayNode->next;
                }

                arrayNode->next = workNode;
                workNode->previous = arrayNode;
            }

            ++tableSize;
        }

        template<typename... Args>
        void emplace(Args... args)
        {
            if (arrayCapacity == 0 && array == nullptr)
            {
                node_pointer* workArray{nullptr};
                node_pointer* workIterator{nullptr};
                const size_type count = 11;
                try
                {
                    workArray = arrayAllocator.allocate(count);
                } catch (std::bad_alloc)
                {
                    return;
                }

                workIterator = workArray;
                for(size_type idx = 0; idx < count; ++idx)
                {
                    *workIterator = nullptr;
                    ++workIterator;
                }

                arrayCapacity = count;
                array = workArray;
            }

            auto loadFactor = load_factor();
            if (loadFactor >= 3.0f)
            {
                reserve(arrayCapacity * 2);
            }

            node_pointer workNode{nullptr};
            node_pointer arrayNode{nullptr};
            try
            {
                workNode = listAllocator.allocate(1);
                new(workNode) node_type{value_type(std::forward<Args>(args)...), nullptr, nullptr};
            } catch (std::bad_alloc)
            {
                return;
            } catch (...)
            {
                (*workNode).~node_type();
                listAllocator.deallocate(workNode, 1);
                return;
            }

            auto key = workNode->data.first;
            auto hashValue = (size_type)hashFun(key) % arrayCapacity;
            arrayNode = array[hashValue];
            if (arrayNode == nullptr)
            {
                array[hashValue] = workNode;
            } else
            {
                while (arrayNode->next != nullptr)
                {
                    arrayNode = arrayNode->next;
                }

                arrayNode->next = workNode;
                workNode->previous = arrayNode;
            }

            ++tableSize;
        }

        iterator find(const key_type& key)
        {
            if (tableSize > 0)
            {
                auto hashValue = (size_type)hashFun(key) % arrayCapacity;
                if (hashValue <= arrayCapacity)
                {
                    return iterator(hashValue, const_cast<HashTable*>(this), array[hashValue]);
                }
            }
            return iterator(arrayCapacity, const_cast<HashTable*>(this), nullptr);
        }

        iterator find(const_reference value)
        {
            if (tableSize > 0)
            {
                const auto key = value.first;
                auto hashValue = (size_type)hashFun(key) % arrayCapacity;
                if (hashValue < arrayCapacity && hashValue >= 0)
                {
                    node_pointer arrayNode = array[hashValue];
                    while (arrayNode != nullptr)
                    {
                        if (arrayNode->data == value)
                        {
                            return iterator(hashValue, const_cast<HashTable*>(this), arrayNode);
                        }
                        arrayNode = arrayNode->next;
                    }
                }
            }
            return iterator(arrayCapacity, const_cast<HashTable*>(this), nullptr);
        }

        iterator find(const mapped_type& value)
        {
            if (tableSize > 0)
            {
                node_pointer arrayNode{nullptr};
                for(size_type idx = 0; idx < arrayCapacity; ++idx)
                {
                    arrayNode = array[idx];
                    while (arrayNode != nullptr)
                    {
                        if (arrayNode->data.second == value)
                        {
                            return iterator(idx, const_cast<HashTable*>(this), arrayNode);
                        }
                        arrayNode = arrayNode->next;
                    }
                }
            }

            return iterator(arrayCapacity, const_cast<HashTable*>(this), nullptr);
        }

        void remove(const key_type& key)
        {
            if (tableSize > 0)
            {
                auto hashValue = (size_type)hashFun(key) % arrayCapacity;
                if (hashValue <= arrayCapacity)
                {
                    node_pointer arrayNode = array[hashValue];
                    node_pointer deleteNode = arrayNode;
                    while (arrayNode != nullptr)
                    {
                        arrayNode = arrayNode->next;
                        (*deleteNode).~node_type();
                        listAllocator.deallocate(arrayNode, 1);
                        deleteNode = arrayNode;
                        --tableSize;
                    }

                    array[hashValue] = nullptr;
                }
            }
        }

        void remove(iterator removeIterator)
        {
            node_pointer removeNode = removeIterator.get_node();
            if (removeNode->previous == nullptr)
            {
                size_type hashValue = (size_type)hashFun(removeIterator.first) % arrayCapacity;
                node_pointer nextNode = removeNode->next;
                (*removeNode).~node_type();
                listAllocator.deallocate(removeNode, 1);
                --tableSize;
                if (nextNode != nullptr)
                {
                    nextNode->previous = nullptr;
                }
                array[hashValue] = nextNode;
            } else
            {
                node_pointer previousNode = removeNode->previous;
                node_pointer nextNode = removeNode->next;
                (*removeNode).~node_type();
                listAllocator.deallocate(removeNode, 1);
                --tableSize;
                previousNode->next = nextNode;
                if (nextNode != nullptr)
                {
                    nextNode->previous = previousNode;
                }
            }
        }

        void remove(const mapped_type& value)
        {
            iterator findIterator = find(value);
            if (findIterator != end())
            {
                node_pointer removeNode = findIterator.get_node();
                if (removeNode->previous == nullptr)
                {
                    size_type hashValue = (size_type)hashFun(findIterator.first) % arrayCapacity;
                    node_pointer nextNode = removeNode->next;
                    (*removeNode).~node_type();
                    listAllocator.deallocate(removeNode, 1);
                    --tableSize;
                    nextNode->previous = nullptr;
                    array[hashValue] = nextNode;
                } else
                {
                    node_pointer previousNode = removeNode->previous;
                    node_pointer nextNode = removeNode->next;
                    (*removeNode).~node_type();
                    listAllocator.deallocate(removeNode, 1);
                    --tableSize;
                    previousNode->next = nextNode;
                    if (nextNode != nullptr)
                    {
                        nextNode->previous = previousNode;
                    }
                }
            }
        }

        void clear()
        {
            if (tableSize > 0)
            {
                node_pointer arrayNode{nullptr};
                node_pointer deleteNode{nullptr};

                for(size_type idx = 0; idx < arrayCapacity; ++idx)
                {
                    arrayNode = array[idx];
                    deleteNode = arrayNode;
                    while (arrayNode != nullptr)
                    {
                        arrayNode = arrayNode->next;
                        (*deleteNode).~node_type();
                        listAllocator.deallocate(deleteNode, 1);
                        deleteNode = arrayNode;
                    }
                }
                tableSize = 0;
            }
        }

        [[nodiscard]] inline bool operator == (const HashTable& other) const noexcept
        {
            bool equal = true;

            if (tableSize == other.tableSize)
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

        [[nodiscard]] inline bool operator != (const HashTable& other) const noexcept
        {
            bool equal = false;
            if (tableSize == other.tableSize)
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

        [[nodiscard]] inline bool operator >= (const HashTable& other) const noexcept
        {
            bool equal;

            if (tableSize == other.tableSize)
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
            } else if (tableSize > other.tableSize)
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

            } else if (tableSize < other.tableSize)
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

        [[nodiscard]] inline bool operator > (const HashTable& other) const noexcept
        {
            bool equal;

            if (tableSize == other.tableSize)
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
            } else if (tableSize > other.tableSize)
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
            } else if (tableSize < other.tableSize)
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

        [[nodiscard]] inline bool operator <= (const HashTable& other) const noexcept
        {
            bool equal;

            if (tableSize == other.tableSize)
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

            } else if (tableSize > other.tableSize)
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

            } else if (tableSize < other.tableSize)
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

        [[nodiscard]] inline bool operator < (const HashTable& other) const noexcept
        {
            bool equal;

            if (tableSize == other.tableSize)
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
            } else if (tableSize > other.tableSize)
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

            } else if (tableSize < other.tableSize)
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

        array_allocator_type arrayAllocator;
        list_allocator_type listAllocator;
        hasher hashFun;
        key_equal keyEqual;
        size_type tableSize;
        size_type arrayCapacity;
        node_pointer* array;
    };

}

#endif //INC_3DGAME_HASH_TABLE_H
