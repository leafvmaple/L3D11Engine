#pragma once

namespace L3D
{
    template<typename T>
    class resource_ptr
    {
    public:
        ~resource_ptr() {
            if (_ptr) {
                _ptr->Release();
            }
        }

        T* get() {
            return _ptr;
        }

        T* operator->() {
            return _ptr;
        }
        T** operator&() {
            return &_ptr;
        }

    private:
        T* _ptr = nullptr;
    };

    template<typename T>
    resource_ptr<T> make_resource() {
        return resource_ptr<T>();
    }
}
