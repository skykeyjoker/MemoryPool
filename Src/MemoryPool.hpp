#ifndef MEMORYPOOL_MEMORYPOOL_HPP
#define MEMORYPOOL_MEMORYPOOL_HPP

#include <climits>
#include <cstddef>

namespace Diana {

	template<typename T, size_t BlockSize = 4096>
	class MemoryPool {
	public:
		typedef T* pointer;

		// rebind<U>::other
		template<typename U>
		struct rebind {
			typedef MemoryPool<U> other;
		};

		MemoryPool() noexcept {
			currentBlock_ = nullptr;
			currentSlot_ = nullptr;
			lastSlot_ = nullptr;
			freeSlot_ = nullptr;
		}

		~MemoryPool() noexcept {
			// 循环销毁内存池中分配的内存区块
			slot_pointer_ curr = currentBlock_;
			while (curr != nullptr) {
				slot_pointer_ prev = curr->next;
				operator delete(reinterpret_cast<void*>(curr));
				curr = prev;
			}
		}

		pointer allocate(size_t = 1, const T* hint = 0) {
			// 如果有空闲的对象槽，直接将空闲区域交付出去
			if (freeSlot_ != nullptr) {
				pointer result = reinterpret_cast<pointer>(freeSlot_);
				freeSlot_ = freeSlot_->next;
				return result;
			} else {
				// 若对象槽不够用，分配一个新的内存区块
				if (currentSlot_ >= lastSlot_) {
					data_pointer_ newBlock = reinterpret_cast<data_pointer_>(operator new(BlockSize));
					reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;
					currentBlock_ = reinterpret_cast<slot_pointer_>(newBlock);
					// 填补整个区块来满足元素内存区域对齐的要求
					data_pointer_ body = newBlock + sizeof(slot_pointer_);
					uintptr_t result = reinterpret_cast<uintptr_t>(body);
					size_t bodyPadding = (alignof(slot_type_) - result) % (alignof(slot_type_));
					currentSlot_ = reinterpret_cast<slot_pointer_>(body + bodyPadding);
					lastSlot_ = reinterpret_cast<slot_pointer_>(newBlock + BlockSize - sizeof(slot_type_) + 1);
				}
				return reinterpret_cast<pointer>(currentSlot_++);
			}
		}

		void deallocate(pointer p, size_t n = 1) {
			if (p != nullptr) {
				reinterpret_cast<slot_pointer_>(p)->next = freeSlot_;
				freeSlot_ = reinterpret_cast<slot_pointer_>(p);
			}
		}

		template<typename U, typename... Args>
		void construct(U* p, Args&&... args) {
			new (p) U(std::forward<Args>(args)...);// 折叠表达式 C++17
		}

		template<typename U>
		void destroy(U* p) {
			p->~U();
		}

	private:
		// 存储内存池中的对象槽
		union Slot_ {
			T element;
			Slot_* next;
		};

		// 数据指针
		typedef char* data_pointer_;
		// 对象槽
		typedef Slot_ slot_type_;
		// 对象槽指针
		typedef Slot_* slot_pointer_;

		// 指向当前内存区块
		slot_pointer_ currentBlock_;
		// 指向当前内存区块的一个对象槽
		slot_pointer_ currentSlot_;
		// 指向当前内存区块的最后一个对象槽
		slot_pointer_ lastSlot_;
		// 指向当前内存区块的空闲对象槽
		slot_pointer_ freeSlot_;

		// 检查定义的内存池大小是否过小
		static_assert(BlockSize >= 2 * sizeof(slot_type_), "Block size too small.");
	};

};// namespace Diana


#endif//MEMORYPOOL_MEMORYPOOL_HPP
