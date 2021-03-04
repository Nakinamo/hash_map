#include <stdexcept>
#include <utility>
#include <vector>

template<class ValueType>
class HashMapIterator {
    typename std::vector<std::vector<ValueType>>* Table;
    size_t first, second;
public:
    HashMapIterator() {}
    HashMapIterator(typename std::vector<std::vector<ValueType>>* t, size_t f, size_t s) {
        first = f;
        second = s;
        Table = t;
    }
    bool operator==(const HashMapIterator &Oth) const {
        return first == Oth.first && second == Oth.second && Table == Oth.Table;
    }
    bool operator!=(const HashMapIterator &Oth) const {
        return !(*this == Oth);
    }
    size_t GetFirst() {
        return first;
    }
    size_t GetSecond() {
        return second;
    }
    ValueType& operator*() {
        return (*Table)[first][second];
    }
    ValueType operator*() const {
        return (*Table)[first][second];
    }
    ValueType* operator->() {
        return &(*Table)[first][second];
    }
    ValueType const* operator->() const {
        return &(*Table)[first][second];
    }
    HashMapIterator operator++() {
        ++second;
        if (second >= (*Table)[first].size()) {
            ++first;
            while (first != Table->size() && (*Table)[first].size() == 0) {
                ++first;
            }
            second = 0;
        }
        return *this;
    }
    HashMapIterator operator++(int) {
        HashMapIterator Old = *this;
        operator++();
        return Old;
    }
};

template<class ValueType>
class HashMapConstIterator {
    const std::vector<std::vector<ValueType>>* Table;
    size_t first, second;
public:
    HashMapConstIterator() {}
    HashMapConstIterator(const std::vector<std::vector<ValueType>>* t, size_t f, size_t s) {
        first = f;
        second = s;
        Table = t;
    }
    bool operator==(const HashMapConstIterator &Oth) const {
        return first == Oth.first && second == Oth.second && Table == Oth.Table;
    }
    bool operator!=(const HashMapConstIterator &Oth) const {
        return !(*this == Oth);
    }
    size_t GetFirst() {
        return first;
    }
    size_t GetSecond() {
        return second;
    }
    ValueType operator*() const {
        return (*Table)[first][second];
    }
    ValueType const* operator->() const {
        return &(*Table)[first][second];
    }
    HashMapConstIterator operator++() {
        ++second;
        if (second >= (*Table)[first].size()) {
            ++first;
            while (first != Table->size() && (*Table)[first].size() == 0) {
                ++first;
            }
            second = 0;
        }
        return *this;
    }
    HashMapConstIterator operator++(int) {
        HashMapConstIterator Old = *this;
        operator++();
        return Old;
    }
};

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
    static const size_t LoadFactor = 10;
    static const size_t ResizeFactor = 2;
    Hash HashFunction;
    std::vector<std::vector<std::pair<const KeyType, ValueType>>> Table;
    size_t Sz = 0;
public:
    typedef HashMapIterator<std::pair<const KeyType, ValueType>> iterator;
    typedef HashMapConstIterator<std::pair<const KeyType, ValueType>> const_iterator;
    HashMap() {
        initialize();
    }
    template<class Iterator>
    HashMap(Iterator first, Iterator last) {
        initialize();
        for (auto i = first; i != last; ++i) {
            insert(*i);
        }
    }
    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> List) {
        initialize();
        for (auto i = List.begin(); i != List.end(); ++i) {
            insert(*i);
        }
    }
    HashMap(const Hash& HashFunction): HashFunction(HashFunction) {
        initialize();
    }
    template<class Iterator>
    HashMap(Iterator first, Iterator last, const Hash& HashFunction): HashFunction(HashFunction) {
        initialize();
        for (auto i = first; i != last; ++i) {
            insert(*i);
        }
    }
    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> List, const Hash& HashFunction): HashFunction(HashFunction) {
        initialize();
        for (auto i = List.begin(); i != List.end(); ++i) {
            insert(*i);
        }
    }
    iterator begin() {
        if (empty()) {
            return end();
        }
        return GetIterator();
    }
    iterator end() {
        return GetIterator(Table.size());
    }
    const_iterator begin() const {
        if (empty()) {
            return end();
        }
        return GetIterator();
    }
    const_iterator end() const {
        return GetIterator(Table.size());
    }
    Hash hash_function() const {
        return HashFunction;
    }
    void insert(const std::pair<KeyType, ValueType>& Item) {
        if (find(Item.first) != end()) {
            return;
        }
        rebuild();
        add(Item);
    }
    void erase(const KeyType& Key) {
        rebuild();
        del(Key);
    }
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
    void clear() {
        Sz = 0;
        Table.clear();
        initialize();
    }
    HashMap operator=(const HashMap& Oth) {
        clear();
        for (auto elem : Oth) {
            insert(elem);
        }
        return *this;
    }
    ValueType& operator[](const KeyType& Key) {
        auto j = find(Key);
        if (j == end()) {
            insert({Key, ValueType()});
            j = find(Key);
        }
        return j->second;
    }
    const ValueType& at(const KeyType& Key) const {
        auto j = find(Key);
        if (j == end()) {
            throw std::out_of_range("incorrect key");
        }
        return j->second;
    }
    size_t size() const {
        return Sz;
    }
    bool empty() const {
        return Sz == 0;
    }

private:
    void initialize() {
        Table.resize(1);
    }
    size_t GetHash(const KeyType& Key) const {
        return HashFunction(Key) % Table.size();
    }
    iterator GetIterator(size_t f = 0, size_t s = 0) {
        return iterator(&Table, f, s);
    }
    const_iterator GetIterator(size_t f = 0, size_t s = 0) const {
        return const_iterator(&Table, f, s);
    }
    void add(const std::pair<KeyType, ValueType>& Item) {
        ++Sz;
        size_t i = GetHash(Item.first);
        Table[i].push_back(Item);
    }
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
