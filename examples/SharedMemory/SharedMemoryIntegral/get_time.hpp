
#ifndef TEMPLATE_GET_TIME_HPP
#define TEMPLATE_GET_TIME_HPP


// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com


//Код файлу фзятий в Олега Фаренюка

#ifndef GET_TIME_HPP
#define GET_TIME_HPP

#include <chrono>
#include <cassert>
#include <atomic> // Для std::atomic_thread_fence і std::memory_order_seq_cst

inline std::chrono::steady_clock::time_point get_current_time_fenced() {
    assert(std::chrono::steady_clock::is_steady && "Timer should be steady (monotonic).");

    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::steady_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);

    return res_time;
}

template<class D>
inline long long to_mi_sec(const D& d)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

#endif //GET_TIME_HPP

#endif //TEMPLATE_GET_TIME_HPP
