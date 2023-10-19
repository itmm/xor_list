#pragma once

#include <algorithm>

template<typename V> struct node {
    node* link_;
    V value_;
};

template<typename V> inline node<V>* ptrxor(const node<V>* a, const node<V>* b) {
    return reinterpret_cast<node<V>*>(
        reinterpret_cast<intptr_t>(a) ^ reinterpret_cast<intptr_t>(b)
    );
}

template<typename V> struct iterator {
    node<V>* cur_;
    node<V>* prev_;
    const bool forward_;

    iterator& operator++();

    V& operator*() { return cur_->value_; }

    const V& operator*() const { return cur_->value_; }
};

template<typename V> inline bool operator==(const iterator<V>& a, const iterator<V>& b) {
    return a.cur_ == b.cur_ && a.forward_ == b.forward_;
}

template<typename V> inline bool operator!=(const iterator<V>& a, const iterator<V>& b) {
    return !(a == b);
}

template<typename V> inline iterator<V>& iterator<V>::operator++() {
    if (cur_ != nullptr) {
        auto n { ptrxor(cur_->link_, prev_) };
        prev_ = cur_;
        cur_ = n;
    }
    return *this;
}

template<typename V> struct const_iterator {
    const node<V>* cur_;
    const node<V>* prev_;
    const bool forward_;

    const_iterator& operator++();

    const V& operator*() const { return cur_->value_; }
};

template<typename V> inline bool operator==(const const_iterator<V>& a, const const_iterator<V>& b) {
    return a.cur_ == b.cur_ && a.forward_ == b.forward_;
}

template<typename V> inline bool operator!=(const const_iterator<V>& a, const const_iterator<V>& b) {
    return !(a == b);
}

template<typename V> inline const_iterator<V>& const_iterator<V>::operator++() {
    if (cur_ != nullptr) {
        auto n { ptrxor(cur_->link_, prev_) };
        prev_ = cur_;
        cur_ = n;
    }
    return *this;
}

template<typename V> class xor_list {
        node<V>* first { nullptr };
        node<V>* last { nullptr };

    public:
        xor_list() = default;

        xor_list(const xor_list<V>&) = delete;

        xor_list(xor_list<V>&& other) noexcept {
            std::swap(first, other.first);
            std::swap(last, other.last);
        }

        xor_list(std::initializer_list<V> init);

        ~xor_list() { erase(begin(), end()); }

        xor_list& operator=(const xor_list<V>&) = delete;

        xor_list& operator=(xor_list<V>&& other) noexcept {
            std::swap(first, other.first);
            std::swap(last, other.last);
            return *this;
        }

        explicit operator bool() const { return first != nullptr; }

        iterator<V> begin() { return iterator<V> { first, nullptr, true }; }
        const_iterator<V> begin() const { return const_iterator<V> { first, nullptr, true}; }
        iterator<V> end() { return iterator<V> { nullptr, nullptr, true }; }
        const_iterator<V> end() const { return const_iterator<V> { nullptr, nullptr, true }; }

        iterator<V> rbegin() { return iterator<V> { last, nullptr, false }; }
        const_iterator<V> rbegin() const { return const_iterator<V> { last, nullptr, false }; }
        iterator<V> rend() { return iterator<V> { nullptr, nullptr, false }; }
        const_iterator<V> rend() const { return const_iterator<V> { nullptr, nullptr, false}; }

        iterator<V> insert(iterator<V> i, const V& v);

        void push_back(const V& v) { insert(rbegin(), v); }

        void push_front(const V& v) { insert(begin(), v); }

        void erase(iterator<V> from, iterator<V> to);
};

template<typename V> inline xor_list<V>::xor_list(std::initializer_list<V> init) {
    for (const auto& v : init) { push_back(v); }
}

template<typename V> inline iterator<V> xor_list<V>::insert(iterator<V> i, const V& v) {
    auto n = new node<V> { ptrxor(i.prev_, i.cur_), v };
    if (!*this) {
        i.cur_ = n->link_ = nullptr;
        first = last = n;
    } else {
        if (i.cur_) {
            auto next { ptrxor(i.cur_->link_, i.prev_) };
            i.cur_->link_ = ptrxor(next, n);
        }
        if (i.prev_) {
            auto prevprev { ptrxor(i.prev_->link_, i.cur_) };
            i.prev_->link_ = ptrxor(prevprev, n);
        }

        if (i.cur_ == first && i.forward_) {
            first = n;
        }
        if (i.cur_ == last && !i.forward_) {
            last = n;
        }

    }
    i.prev_ = n;
    return i;
}

template<typename V> inline void xor_list<V>::erase(iterator<V> from, iterator<V> to) {
    auto cur { from };
    while (cur != to) {
        auto old { cur.cur_ };
        ++cur;
        delete old;
    }

    if (first == from.cur_ && from.forward_) { first = to.cur_; }
    if (last == to.prev_ && from.forward_) { last = from.prev_; }
    if (last == from.cur_ && !from.forward_) { last = to.cur_; }
    if (first == to.prev_ && !from.forward_) { first = from.prev_; }

    if (from.prev_) { from.prev_->link_ = ptrxor(ptrxor(from.prev_->link_, from.cur_), to.cur_); }
    if (to.cur_) { to.cur_->link_ = ptrxor(ptrxor(to.cur_->link_, to.prev_), from.prev_); }
    to.prev_ = from.prev_;
    from.cur_ = to.cur_;
}

template<typename V> inline bool operator==(const xor_list<V>& a, const xor_list<V>& b) {
    auto bi { b.begin() }, be { b.end() };
    for (auto ai { a.begin() }, ae { a.end() }; ai != ae; ++ai, ++bi) {
        if (bi == be) { return false; }
        if (*ai != *bi) { return false; }
    }
    return bi == be;
}