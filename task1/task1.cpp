#include <stdexcept>
#include <utility>
#include <vector>

// Hash table iterator.
template<class ValueType>
class HashMapIterator {
    typename std::vector<std::vector<ValueType>>* Table;
    size_t First, Second;
  public:
    // Default constructor.
    HashMapIterator() {}
    // Constructor by table, bucket number and item number.
    HashMapIterator(typename std::vector<std::vector<ValueType>>* t, size_t f, size_t s) {
        First = f;
        Second = s;
        Table = t;
    }
    // Returns true if iterator is equal to Oth in O(1) time.
    bool operator==(const HashMapIterator &Oth) const {
        return First == Oth.first && Second == Oth.second && Table == Oth.Table;
    }
    // Returns true if iterator is not equal to Oth in O(1) time.
    bool operator!=(const HashMapIterator &Oth) const {
        return !(*this == Oth);
    }
    // Returns bucket number in O(1) time.
    size_t GetFirst() {
        return First;
    }
    // Returns item number in bucket in O(1) time.
    size_t GetSecond() {
        return Second;
    }
    // Returns item object in O(1) time.
    ValueType& operator*() {
        return (*Table)[First][Second];
    }
    // Returns constant item object in O(1) time.
    ValueType operator*() const {
        return (*Table)[First][Second];
    }
    // Returns item reference in O(1) time.
    ValueType* operator->() {
        return &(*Table)[First][Second];
    }
    // Returns constant item reference in O(1) time.
    ValueType const* operator->() const {
        return &(*Table)[First][Second];
    }
    // Moves iterator to next item and returns it in O(1) expected time.
    HashMapIterator operator++() {
        ++Second;
        if (Second >= (*Table)[First].size()) {
            ++First;
            while (First != Table->size() && (*Table)[First].size() == 0) {
                ++First;
            }
            Second = 0;
        }
        return *this;
    }
    // Moves iterator to next item and returns its previous state in O(1) expected time.
    HashMapIterator operator++(int) {
        HashMapIterator Old = *this;
        operator++();
        return Old;
    }
};

// Constant hash table iterator.
template<class ValueType>
class HashMapConstIterator {
    const std::vector<std::vector<ValueType>>* Table;
    size_t First, Second;
  public:
    // Default constructor.
    HashMapConstIterator() {}
    // Constructor by table, bucket number and item number.
    HashMapConstIterator(const std::vector<std::vector<ValueType>>* t, size_t f, size_t s) {
        First = f;
        Second = s;
        Table = t;
    }
    // Returns true if constant iterator is equal to Oth in O(1) time.
    bool operator==(const HashMapConstIterator &Oth) const {
        return First == Oth.first && Second == Oth.second && Table == Oth.Table;
    }
    // Returns true if constant iterator is not equal to Oth in O(1) time.
    bool operator!=(const HashMapConstIterator &Oth) const {
        return !(*this == Oth);
    }
    // Returns bucket number in O(1) time.
    size_t GetFirst() {
        return First;
    }
    // Returns item number in bucket in O(1) time.
    size_t GetSecond() {
        return Second;
    }
    // Returns constant item object in O(1) time.
    ValueType operator*() const {
        return (*Table)[First][Second];
    }
    // Returns constant item reference in O(1) time.
    ValueType const* operator->() const {
        return &(*Table)[First][Second];
    }
    // Moves constant iterator to next item and returns it in O(1) expected time.
    HashMapConstIterator operator++() {
        ++Second;
        if (Second >= (*Table)[First].size()) {
            ++First;
            while (First != Table->size() && (*Table)[First].size() == 0) {
                ++First;
            }
            Second = 0;
        }
        return *this;
    }
    // Moves constant iterator to next item and returns its previous state in O(1) expected time.
    HashMapConstIterator operator++(int) {
        HashMapConstIterator Old = *this;
        operator++();
        return Old;
    }
};

// Bucket hash table with load factor = 10 and resize factor = 2.
template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
    Hash HashFunction;
    std::vector<std::vector<std::pair<const KeyType, ValueType>>> Table;
    size_t Sz = 0;
  public:
    static constexpr size_t LoadFactor = 10;
    static constexpr size_t ResizeFactor = 2;
    typedef HashMapIterator<std::pair<const KeyType, ValueType>> iterator;
    typedef HashMapConstIterator<std::pair<const KeyType, ValueType>> const_iterator;
    // Default constructor.
    HashMap() {
        initialize();
    }
    // Constructor for begin and end iterators.
    template<class Iterator>
    HashMap(Iterator first, Iterator last) {
        initialize();
        for (auto i = first; i != last; ++i) {
            insert(*i);
        }
    }
    // Constructor for initializer list.
    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> List) {
        initialize();
        for (auto i = List.begin(); i != List.end(); ++i) {
            insert(*i);
        }
    }
    // Default constructor with custom hash function.
    HashMap(const Hash& HashFunction): HashFunction(HashFunction) {
        initialize();
    }
    // Constructor for begin and end iterators with custom hash function.
    template<class Iterator>
    HashMap(Iterator first, Iterator last, const Hash& HashFunction): HashFunction(HashFunction) {
        initialize();
        for (auto i = first; i != last; ++i) {
            insert(*i);
        }
    }
    // Constructor for initializer list with custom hash function.
    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> List, const Hash& HashFunction): HashFunction(HashFunction) {
        initialize();
        for (auto i = List.begin(); i != List.end(); ++i) {
            insert(*i);
        }
    }
    // Returns begin iterator in O(1) time.
    iterator begin() {
        if (empty()) {
            return end();
        }
        return GetIterator();
    }
    // Returns end iterator in O(1) time.
    iterator end() {
        return GetIterator(Table.size());
    }
    // Returns constanst begin iterator in O(1) time.
    const_iterator begin() const {
        if (empty()) {
            return end();
        }
        return GetIterator();
    }
    // Returns constanst end iterator in O(1) time.
    const_iterator end() const {
        return GetIterator(Table.size());
    }
    // Returns hash function in O(1) time.
    Hash hash_function() const {
        return HashFunction;
    }
    // Inserts item in O(1) amortized time. May invalidate iterators.
    void insert(const std::pair<KeyType, ValueType>& Item) {
        if (find(Item.first) != end()) {
            return;
        }
        rebuild();
        add(Item);
    }
    // Erases item in O(1) amortized time. May invalidate iterators.
    void erase(const KeyType& Key) {
        rebuild();
        del(Key);
    }
    // Returns iterator of item by key in O(bucket size). If hash function is good, works in O(1) expected time.
    iterator find(const KeyType& Key) {
        if (empty()) {
            return end();
        }
        size_t i = GetHash(Key);
        for (size_t j = 0; j != Table[i].size(); ++j) {
            if (Table[i][j].first == Key) {
                return GetIterator(i, j);
            }
        }
        return end();
    }
    // Returns constant iterator of item by key in O(bucket size). If hash function is good, works in O(1) expected time.
    const_iterator find(const KeyType& Key) const {
        if (empty()) {
            return end();
        }
        size_t i = GetHash(Key);
        for (size_t j = 0; j != Table[i].size(); ++j) {
            if (Table[i][j].first == Key) {
                return GetIterator(i, j);
            }
        }
        return end();
    }
    // Clears table in O(size) time.
    void clear() {
        Sz = 0;
        Table.clear();
        initialize();
    }
    // Assignment operator. Works in O(Oth.size) time.
    HashMap operator=(const HashMap& Oth) {
        clear();
        for (auto elem : Oth) {
            insert(elem);
        }
        return *this;
    }
    // Returns item by key in O(1) time. If no items stored by this key, creates a new item.
    ValueType& operator[](const KeyType& Key) {
        auto j = find(Key);
        if (j == end()) {
            insert({Key, ValueType()});
            j = find(Key);
        }
        return j->second;
    }
    // Returns item by key in O(1) time. If no items stored by this key, throws out of range exception. If an exception is thrown, there are no changes in the container.
    const ValueType& at(const KeyType& Key) const {
        auto j = find(Key);
        if (j == end()) {
            throw std::out_of_range("incorrect key");
        }
        return j->second;
    }
    // Returns number of stored items in O(1) time.
    size_t size() const {
        return Sz;
    }
    // Returns true if the container does not store any item in O(1) time.
    bool empty() const {
        return Sz == 0;
    }

  private:
    // Resizes table to one bucket.
    void initialize() {
        Table.resize(1);
    }
    // Returns hash function of a key in O(1) time.
    size_t GetHash(const KeyType& Key) const {
        return HashFunction(Key) % Table.size();
    }
    // Returns iterator for bucket of number f and item of number s in the bucket in O(1) time.
    iterator GetIterator(size_t f = 0, size_t s = 0) {
        return iterator(&Table, f, s);
    }
    // Returns constant iterator for bucket of number f and item of number s in the bucket in O(1) time.
    const_iterator GetIterator(size_t f = 0, size_t s = 0) const {
        return const_iterator(&Table, f, s);
    }
    // Inserts item in bucket of its hash in O(1) time.
    void add(const std::pair<KeyType, ValueType>& Item) {
        ++Sz;
        size_t i = GetHash(Item.first);
        Table[i].push_back(Item);
    }
    // Erases item from bucket of its hash in O(bucket size) time. If hash function is good, works in expected O(1) time.
    void del(const KeyType& Key) {
        size_t i = GetHash(Key);
        std::vector<std::pair<const KeyType, ValueType>> NewBucket;
        for (auto elem : Table[i]) {
            if (elem.first != Key) {
                NewBucket.push_back(elem);
            }
        }
        if (NewBucket.size() != Table[i].size()) {
            swap(Table[i], NewBucket);
            --Sz;
        }
    }
    // Rebuilds table with 'resize factor' times more buckets if number of items stored is more than product of load factor and buckets number in O(size) time.
    // Rebuilds table with 'resize factor' times less buckets if product of number of items stored and load factor is less than buckets number in O(size) time.
    void rebuild() {
        if (size() >= Table.size() * LoadFactor) {
            Sz = 0;
            std::vector<std::vector<std::pair<const KeyType, ValueType>>> OldTable;
            swap(OldTable, Table);
            Table.resize(OldTable.size() * ResizeFactor);
            for (auto& Bucket : OldTable) {
                for (auto& Item : Bucket) {
                    add(Item);
                }
            }
        } else if (size() * LoadFactor < Table.size()) {
            Sz = 0;
            std::vector<std::vector<std::pair<const KeyType, ValueType>>> OldTable;
            swap(OldTable, Table);
            Table.resize(OldTable.size() / ResizeFactor);
            for (auto& Bucket : OldTable) {
                for (auto& Item : Bucket) {
                    add(Item);
                }
            }
        }
        if (Table.size() == 0) {
            initialize();
        }
    }
};
