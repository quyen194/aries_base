/********************************************************************
  Copyright 2025, Cong Quyen Knight. All rights reserved

  project:   Aries Base
  author:    quyen19492
  email:     quyen19492@gmail.com

  created:   2025/11/16 11:57
  filename:  aries_base/container/linked_list.hpp

  purpose:   Ring linked list implementation
             Cloned from the implementation of Google Chromium's base::LinkedList
*********************************************************************/


// -----------------------------------------------------------------------------
#ifndef ARIES_BASE_CONTAINER_LINKED_LIST_HPP
#define ARIES_BASE_CONTAINER_LINKED_LIST_HPP
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
#include "aries_base/definitions/macro.hpp"
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
namespace container {
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Simple LinkedList type. (See the Q&A section to understand how this
// differs from std::list).
//
// To use, start by declaring the class which will be contained in the linked
// list, as extending LinkNode (this gives it next/previous pointers).
//
//   class MyNodeType : public LinkNode<MyNodeType> {
//     ...
//   };
//
// Next, to keep track of the list's head/tail, use a LinkedList instance:
//
//   LinkedList<MyNodeType> list;
//
// To add elements to the list, use any of LinkedList::Append,
// LinkNode::InsertBefore, or LinkNode::InsertAfter:
//
//   LinkNode<MyNodeType>* n1 = ...;
//   LinkNode<MyNodeType>* n2 = ...;
//   LinkNode<MyNodeType>* n3 = ...;
//
//   list.Append(n1);
//   list.Append(n3);
//   n3->InsertBefore(n3);
//
// Lastly, to iterate through the linked list forwards:
//
//   for (LinkNode<MyNodeType>* node = list.head();
//        node != list.end();
//        node = node->next()) {
//     MyNodeType* value = node->value();
//     ...
//   }
//
// Or to iterate the linked list backwards:
//
//   for (LinkNode<MyNodeType>* node = list.tail();
//        node != list.end();
//        node = node->previous()) {
//     MyNodeType* value = node->value();
//     ...
//   }
//
// Questions and Answers:
//
// Q. Should I use std::list or LinkedList?
//
// A. The main reason to use LinkedList over std::list is
//    performance. If you don't care about the performance differences
//    then use an STL container, as it makes for better code readability.
//
//    Comparing the performance of LinkedList<T> to std::list<T*>:
//
//    * Erasing an element of type T* from LinkedList<T> is
//      an O(1) operation. Whereas for std::list<T*> it is O(n).
//      That is because with std::list<T*> you must obtain an
//      iterator to the T* element before you can call erase(iterator).
//
//    * Insertion operations with LinkedList<T> never require
//      heap allocations.
//
// Q. How does LinkedList implementation differ from std::list?
//
// A. Doubly-linked lists are made up of nodes that contain "next" and
//    "previous" pointers that reference other nodes in the list.
//
//    With LinkedList<T>, the type being inserted already reserves
//    space for the "next" and "previous" pointers (LinkNode<T>*).
//    Whereas with std::list<T> the type can be anything, so the implementation
//    needs to glue on the "next" and "previous" pointers using
//    some internal node type.
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

template <typename T>
class LinkNode {
 public:
  LinkNode() : previous_(nullptr), next_(nullptr) {}
  LinkNode(LinkNode<T>* previous, LinkNode<T>* next)
      : previous_(previous), next_(next) {}
  virtual ~LinkNode() { RemoveFromList(); }

  // Insert |this| into the linked list, before |e|.
  void InsertBefore(LinkNode<T>* e) {
    this->next_ = e;
    this->previous_ = e->previous_;
    // do linkedlisk có cấu trúc danh sách liên kết vòng
    // nên trường hợp e->previous_ = nullptr là ko xảy ra
    e->previous_->next_ = this;
    e->previous_ = this;
  }

  // Insert |this| into the linked list, after |e|.
  void InsertAfter(LinkNode<T>* e) {
    this->next_ = e->next_;
    this->previous_ = e;
    // do linkedlisk có cấu trúc danh sách liên kết vòng
    // nên trường hợp e->next_ = nullptr là ko xảy ra
    e->next_->previous_ = this;
    e->next_ = this;
  }

  // Remove |this| from the linked list.
  void RemoveFromList() {
    if (this->previous_) {
      this->previous_->next_ = this->next_;
    }
    if (this->next_) {
      this->next_->previous_ = this->previous_;
    }
    // next() and previous() return non-NULL if and only this node is not in any
    // list.
    this->next_ = nullptr;
    this->previous_ = nullptr;
  }

  // DANGEROUS: make sure you create this with new operator before call this
  void Free() {
    RemoveFromList();
    T * v = value();
    delete(v);
  }

  LinkNode<T>* previous() const {
    return previous_;
  }

  LinkNode<T>* next() const {
    return next_;
  }

  // Cast from the node-type to the value type.
  const T* value() const {
    return static_cast<const T*>(this);
  }

  T* value() {
    return static_cast<T*>(this);
  }

 private:
  LinkNode<T>* previous_;
  LinkNode<T>* next_;

 private:
  DISALLOW_COPY_AND_ASSIGN(LinkNode);
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------

template <typename T>
class LinkedList {
 public:
  // The "root" node is self-referential, and forms the basis of a circular
  // list (root_.next() will point back to the start of the list,
  // and root_->previous() wraps around to the end of the list).
  LinkedList() : root_(&root_, &root_) {}
  virtual ~LinkedList() {
    FreeAll();
  }

  // Appends |e| to the end of the linked list.
  void Append(LinkNode<T>* e) {
    e->InsertBefore(&root_);
  }

  uint32_t size() {
    uint32_t count = 0;
    for (auto it : *this) {
      it;  // suppress compiler warning
      count++;
    }
    return count;
  }

  LinkNode<T>* head() const {
    return root_.next();
  }

  LinkNode<T>* tail() const {
    return root_.previous();
  }

  const LinkNode<T>* end_list() const {
    return &root_;
  }

  bool empty() const {
    return head() == end_list();
  }

  void FreeAll() {
    while (!empty()) {
      head()->Free();
    }
  }

  // Iterator class
  class iterator {
   public:
    iterator(LinkNode<T>* n) : node_(n) {}

    T* operator*() { return node_->value(); }

    iterator& operator++() {
      node_ = node_->next();
      return *this;
    }

    bool operator==(const iterator& other) const {
      return node_ == other.node_;
    }
    bool operator!=(const iterator& other) const {
      return node_ != other.node_;
    }

   private:
    LinkNode<T>* node_;
  };

  iterator begin() { return iterator(root_.next()); }
  iterator end() { return iterator(&root_); }

  // Reverse iterator class
  class reverse_iterator {
   public:
    reverse_iterator(LinkNode<T>* n) : node_(n) {}

    T* operator*() { return node_->value(); }

    reverse_iterator& operator++() {
      node_ = node_->previous();
      return *this;
    }

    bool operator==(const reverse_iterator& other) const {
      return node_ == other.node_;
    }
    bool operator!=(const reverse_iterator& other) const {
      return node_ != other.node_;
    }

   private:
    LinkNode<T>* node_;
  };

  reverse_iterator rbegin() { return reverse_iterator(root_.previous()); }
  reverse_iterator rend() { return reverse_iterator(&root_); }

 private:
  LinkNode<T> root_;

 private:
  DISALLOW_COPY_AND_ASSIGN(LinkedList);
};
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
}  // namespace container
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif  // ARIES_BASE_CONTAINER_LINKED_LIST_HPP
// -----------------------------------------------------------------------------
