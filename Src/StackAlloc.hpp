#ifndef MEMORYPOOL_STACKALLOC_HPP
#define MEMORYPOOL_STACKALLOC_HPP

#include <memory>

namespace Diana {
	template<typename T>
	struct StackNode_ {
		T data;
		StackNode_* prev;
	};

	// T为储存的对象类型，Alloc为使用的分配器（默认使用std::allocator）
	template<typename T, typename Alloc = std::allocator<T>>
	class StackAlloc {
	public:
		typedef StackNode_<T> Node;
		typedef typename Alloc::template rebind<Node>::other allocator;

		StackAlloc() { head_ = 0; }
		~StackAlloc() { clear(); }

		// 当栈中元素为空时返回true
		bool empty() { return (head_ == 0); };

		// 释放栈中所有元素
		void clear() {
			Node* curr = head_;
			// 依次出栈
			while (curr != 0) {
				Node* tmp = curr->prev;
				allocator_.destroy(curr);
				allocator_.deallocate(curr, 1);
				curr = tmp;
			}
			head_ = 0;
		}

		// 压栈
		void push(T element) {
			// 为一个节点分配内存
			Node* newNode = allocator_.allocate(1);
			// 调用节点的构造函数
			allocator_.construct(newNode, Node());

			// 入栈操作
			newNode->data = element;
			newNode->prev = head_;
			head_ = newNode;
		}

		// 出栈
		T pop() {
			// 出栈返回元素
			T result = head_->data;
			Node* tmp = head_->prev;
			allocator_.destroy(head_);
			allocator_.deallocate(head_, 1);
			head_ = tmp;
			return result;
		}

		// 返回栈顶元素
		T top() { return head_->data; }

	private:
		allocator allocator_;
		Node* head_;
	};

};// namespace Diana

#endif//MEMORYPOOL_STACKALLOC_HPP
