#pragma once
#include <cstdlib>// malloc and free
#include <cstring>// memcpy
#include <new>    // bad_alloc

namespace mystl {

    //һ��������
    class _malloc_alloc {
    public:
        // alias declaration
        using malloc_handler = void (*)();

    private:
        //���º���ָ�����Դ����ڴ治������
        static void* oom_malloc(size_t);
        static void* oom_realloc(void*, size_t);
        static malloc_handler _malloc_alloc_oom_handler;

    public:
        static void* allocate(size_t n) {
            void* result = malloc(n);
            if (result == nullptr) result = oom_malloc(n);
            return result;
        }

        static void deallocate(void* p, size_t /*n*/) { free(p); }

        static void* reallocate(void* p, size_t /*old_sz*/, size_t new_sz) {
            void* result = realloc(p, new_sz);
            if (result == nullptr) oom_realloc(p, new_sz);
            return result;
        }

        //ģ��set_new_handler
        //ԭ�����ڲ�δʹ��new��delete�����ڴ棬����޷�ʹ��set_new_handler
        static malloc_handler set_malloc_handler(malloc_handler f) {
            malloc_handler old = _malloc_alloc_oom_handler;
            _malloc_alloc_oom_handler = f;
            return old;
        }
    };

    inline void* _malloc_alloc::oom_malloc(size_t n) {
        malloc_handler new_alloc_handler;
        void* result;
        for (;;) {//���ϳ����ͷš�����
            new_alloc_handler = _malloc_alloc_oom_handler;
            if (!new_alloc_handler) throw std::bad_alloc();
            (*new_alloc_handler)();//����handler����ͼ�ͷ��ڴ�
            result = malloc(n);
            if (result) return result;
        }
    }

    inline void* _malloc_alloc::oom_realloc(void* p, size_t n) {
        malloc_handler new_alloc_handler;
        void* result;
        for (;;) {
            new_alloc_handler = _malloc_alloc_oom_handler;
            if (!new_alloc_handler) throw std::bad_alloc();
            (*new_alloc_handler)();
            result = realloc(p, n);
            if (result) return result;
        }
    }

    using malloc_alloc = _malloc_alloc;

    // freelist�����趨
    //�����ϵ��߽磬�������ޣ�freelist����
    // Effective C++����enum���÷�
    enum _freelist_setting {
        ALIGN = 8,
        MAX_BYTES = 128,
        NFREELISTS = MAX_BYTES / ALIGN
    };

    // �ڶ���������
    class _default_alloc {
    private:
        // ��bytes�ϵ���8�ı���
        static size_t ROUND_UP(size_t bytes) {
            return (((bytes)+static_cast<size_t>(ALIGN) - 1) & ~(static_cast<size_t>(ALIGN) - 1));
        }

    private:
        // free_list�ڵ�
        // ����union���ԣ�������Ҫռ�ö�����ڴ�
        union obj {
            union obj* free_list_link;//ָ����һ���ڵ�
            char client_data[1];      //ָ����Դ
        };

    private:
        static obj* volatile free_list[NFREELISTS];
        // ����ʹ�õڼ��Žڵ㣬��1����
        static size_t FREELIST_INDEX(size_t bytes) {
            return (bytes + static_cast<size_t>(ALIGN) - 1) / static_cast<size_t>(ALIGN) - 1;
        }
        // ����һ����СΪn�Ķ��󣬲��ҿ��ܼ����СΪn���������鵽free_list
        static void* refill(size_t n);
        // ����һ���ռ䣬������nobjs����СΪsize������
        // ����������� nobjs���ܻή��
        static char* chunk_alloc(size_t size, int& nobjs);

        // chunk allocation state
        static char* start_free;//�ڴ����ʼλ�ã�ֻ��chunk_alloc()�б仯
        static char* end_free;  //�ڴ�ؽ���λ�ã�ֻ��chunk_alloc()�б仯
        static size_t heap_size;

    public:
        static void* allocate(size_t n);
        static void deallocate(void* p, size_t n);
        static void* reallocate(void* p, size_t old_sz, size_t new_sz);
    };

    // ��free_list�޿�������ʱ���������ռ�
    // �¿ռ�ȡ���ڴ�أ�Ĭ�ϻ�ȡ20���ڵ�(���飩
    // ���ڴ�ز��㣬���ȡ�Ľ�С��20
    inline void* _default_alloc::refill(size_t n) {
        int nobjs = 20;
        // ���Ե���chunk_alloc,ע��nobjs��pass-by-reference����
        char* chunk = chunk_alloc(n, nobjs);
        obj* volatile* my_free_list;
        obj* result;
        obj* current_obj, * next_obj;

        // ��ֻ��ȡ��һ��������ֱ�ӷ���������ߣ�������free_list
        if (1 == nobjs) return (chunk);
        my_free_list = free_list + FREELIST_INDEX(n);

        // ��chunk�ռ��ڽ���free_list
        result = reinterpret_cast<obj*>(chunk);
        // ����free_listָ���ڴ�ط���Ŀռ�
        // chunkָ����ڴ�ֱ�ӷָ��û���free_listָ��ʣ�£�19����٣�������
        *my_free_list = next_obj = reinterpret_cast<obj*>(chunk + n);
        for (int i = 1;; ++i) {
            current_obj = next_obj;
            next_obj =
                reinterpret_cast<obj*>(reinterpret_cast<char*>(next_obj) + n);
            if (nobjs - 1 == i) {
                current_obj->free_list_link = nullptr;
                break;
            }
            else {
                current_obj->free_list_link = next_obj;
            }
        }
        return result;
    }

    // Ĭ��sizeΪ8��������
    inline char* _default_alloc::chunk_alloc(size_t size, int& nobjs) {
        char* result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;//�ڴ��ʣ��ռ�
        if (bytes_left >= total_bytes) {
            // ������������
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        else if (bytes_left > size) {
            // ������������һ����������
            nobjs = static_cast<int>(bytes_left / size);
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        else {
            // �ڴ��һ�����鶼�޷��ṩ
            size_t bytes_to_get =
                2 * total_bytes + ROUND_UP(heap_size >> 4);// ��heap����ע����ڴ棬heap_size���������ô������Ӷ�����
            if (bytes_left > 0) {
                // ��ǰ�ڴ�ػ���һ�����ڴ棬Ϊ�˲��˷ѷ����free_list
                obj* volatile* my_free_list =
                    free_list + FREELIST_INDEX(bytes_left);
                reinterpret_cast<obj*>(start_free)->free_list_link = *my_free_list;
                *my_free_list = reinterpret_cast<obj*>(start_free);
            }
            // ����heap�ռ��Բ����ڴ��
            start_free = reinterpret_cast<char*>(malloc(bytes_to_get));
            if (!start_free) {
                // heap�ռ䲻�㣬mallocʧ��
                obj* volatile* my_free_list;
                obj* p;
                // ��free_list�м���Ƿ��з������������
                for (size_t i = size; i <= static_cast<size_t>(MAX_BYTES);
                    i += static_cast<size_t>(ALIGN)) {
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if (p) {
                        // �������Է��������
                        *my_free_list = p->free_list_link;// ���뵱ǰ����
                        start_free = reinterpret_cast<char*>(p);
                        end_free = start_free + i;
                        return (chunk_alloc(
                            size,
                            nobjs));// �ݹ����������nobjs����ʱ��Ȼ����else_if��֧
                    }
                }
                end_free = nullptr;// �������Ҳ����ڴ�
                // ���õ�һ���������۲����ܷ�����ڴ棬���׳��쳣
                start_free =
                    reinterpret_cast<char*>(malloc_alloc::allocate(bytes_to_get));
            }
            heap_size += bytes_to_get;// ��ռ�õĶ��ڴ�
            end_free = start_free + bytes_to_get;
            return chunk_alloc(size, nobjs);// ��������������nobjs
        }
    }

    inline void* _default_alloc::allocate(size_t n) {
        obj* volatile* my_free_list;
        obj* result;
        // ��n����128,����õ�һ��������
        if (n > MAX_BYTES) return (malloc_alloc::allocate(n));
        // ѡ����õڼ�����
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (result == nullptr) {
            // δ�ҵ�����free_list��׼�����free_list
            void* r = refill(ROUND_UP(n));
            return r;
        }
        // ����freelist
        *my_free_list = result->free_list_link;
        return result;
    }

    inline void _default_alloc::deallocate(void* p, size_t n) {
        // p����Ϊnullptr
        if (n > static_cast<size_t>(MAX_BYTES))
            malloc_alloc::deallocate(p, n);
        else {
            // Ѱ�Ҷ�Ӧ��free list
            obj* volatile* my_free_list = free_list + FREELIST_INDEX(n);
            obj* q = reinterpret_cast<obj*>(p);
            // ����free list����������
            q->free_list_link = *my_free_list;
            *my_free_list = q;
        }
    }

    inline void* _default_alloc::reallocate(void* p, size_t old_sz, size_t new_sz) {
        void* result;
        size_t copy_sz;

        if (old_sz > static_cast<size_t>(MAX_BYTES) && new_sz > static_cast<size_t>(MAX_BYTES))
            return realloc(p, new_sz);
        if (ROUND_UP(old_sz) == ROUND_UP(new_sz)) return p;
        result = allocate(new_sz);
        copy_sz = new_sz > old_sz ? old_sz : new_sz;
        memcpy(result, p, copy_sz);
        deallocate(p, old_sz);
        return result;
    }

}// namespace mystl