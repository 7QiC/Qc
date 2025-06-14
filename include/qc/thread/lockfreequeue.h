#ifndef QC_LOCKFREEQUEUE_H
#define QC_LOCKFREEQUEUE_H

#include <atomic>
#include <memory>
#include "qc/macro.h"

namespace qc {
namespace thread {

template<typename T>
class LockFreeQueue {
  public:
    using ptr = std::unique_ptr<LockFreeQueue<T>>;
    LockFreeQueue() {
        Node* dummy = new Node();
        head.store(dummy);
        tail.store(dummy);
    }

    ~LockFreeQueue() {
        while (Node* tmp = head.load(std::memory_order_relaxed)) {
            head.store(tmp->next.load(std::memory_order_relaxed), std::memory_order_relaxed);
            delete tmp;
        }
    }

    void enqueue(T value) {
        Node* newNode = new Node(std::move(value));
        Node* curTail = nullptr;
        Node* next = nullptr;
        
        while (true) {
            curTail = tail.load(std::memory_order_acquire);
            next = curTail->next.load(std::memory_order_acquire);

            if (curTail == tail.load(std::memory_order_relaxed)) {
                if (next == nullptr) {
                    // 修正：使用 release 内存顺序
                    if (curTail->next.compare_exchange_weak(
                        next, newNode, 
                        std::memory_order_release, 
                        std::memory_order_relaxed)) {
                        break;
                    }
                } else {
                    // 帮助推进 tail
                    tail.compare_exchange_weak(
                        curTail, next,
                        std::memory_order_release,
                        std::memory_order_relaxed);
                }
            }
        }
        
        // 尝试更新 tail，失败也没关系（其他线程可能已经更新了）
        tail.compare_exchange_weak(
            curTail, newNode,
            std::memory_order_release,
            std::memory_order_relaxed);
    }

    T dequeue() {
        Node* curHead = nullptr;
        Node* curTail = nullptr;
        Node* next = nullptr;
        T ret;

        while (true) {
            curHead = head.load(std::memory_order_acquire);
            curTail = tail.load(std::memory_order_acquire);
            next = curHead->next.load(std::memory_order_acquire);

            if (curHead == head.load(std::memory_order_relaxed)) {
                if (curHead == curTail) {
                    if (next == nullptr) {
                        return nullptr;  // 队列为空
                    }
                    // 帮助推进 tail
                    tail.compare_exchange_weak(
                        curTail, next,
                        std::memory_order_release,
                        std::memory_order_relaxed);
                } else {
                    ret = next->data;
                    
                    // 尝试移动 head
                    if (head.compare_exchange_weak(
                        curHead, next,
                        std::memory_order_release,
                        std::memory_order_relaxed)) {
                        break;
                    }
                }
            }
        }

        // 安全删除旧头节点
        delete curHead;
        return ret;
    }

    bool empty() const {
        Node* h = head.load(std::memory_order_acquire);
        Node* t = tail.load(std::memory_order_acquire);
        Node* next = h->next.load(std::memory_order_acquire);

        return (h == t) && (next == nullptr);
    }

  private:
    struct Node {
        T data;
        std::atomic<Node*> next;

        Node() : next(nullptr) {}
        explicit Node(T value) : data(std::move(value)), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;
};

}
}

#endif