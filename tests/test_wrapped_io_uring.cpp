/**
 * Catch2 unit tests for modules/wrappers/wrapped_io_uring.cppm
 *
 * @author    Simon Gleissner <simon@gleissner.de>, http://guardfw.de
 * @copyright MIT license, see file LICENSE
 * @file
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <cstdint>
#include <source_location>
#include <sstream>
#include <linux/io_uring.h>  // io_uring_params
#include <atomic>            // std::atomic_ref<>
#include <expected>          // std::expected<>
#include <cstring>           // memset()
#include <alloca.h>          // alloca()
#include <system_error>      // std::system_error

import guardfw.wrapped_io_uring;
import guardfw.wrapped_unistd; // GuardFW::close()
import guardfw.wrapped_mman;   // GuardFW::mmap(), GuardFW::munmap()

namespace
{

consteval std::source_location fixed_location()
{
    return std::source_location::current();
}

}  // namespace

constexpr static std::source_location fixloc = fixed_location();


TEST_CASE("io_uring exceptions", "[io_uring]")
{
    std::ostringstream what_setup;
    what_setup << "in function '" << fixloc.function_name()  // copied from ContextPosix<>::throw_function()
               << "' in file '" << fixloc.file_name()        // copied from ContextPosix<>::throw_function()
               << "' at line " << fixloc.line()              // copied from ContextPosix<>::throw_function()
               << ": wrapped call to 'syscall()' failed with error 14: Bad address";

    std::ostringstream what_register_enter;
    what_register_enter << "in function '" << fixloc.function_name()  // copied from ContextPosix<>::throw_function()
                        << "' in file '" << fixloc.file_name()        // copied from ContextPosix<>::throw_function()
                        << "' at line " << fixloc.line()              // copied from ContextPosix<>::throw_function()
                        << ": wrapped call to 'syscall()' failed with error 9: Bad file descriptor";

    CHECK_THROWS_AS(GuardFW::io_uring_setup(256, nullptr, fixloc), std::system_error);
    CHECK_THROWS_WITH(GuardFW::io_uring_setup(256, nullptr, fixloc), what_setup.str());

    CHECK_THROWS_AS(GuardFW::io_uring_register(-1, IORING_UNREGISTER_BUFFERS, nullptr, 0, fixloc), std::system_error);
    CHECK_THROWS_WITH(
        GuardFW::io_uring_register(-1, IORING_UNREGISTER_BUFFERS, nullptr, 0, fixloc), what_register_enter.str()
    );

    CHECK_THROWS_AS(GuardFW::io_uring_enter(-1, 0, 0, 0, nullptr, fixloc), std::system_error);
    CHECK_THROWS_WITH(GuardFW::io_uring_enter(-1, 0, 0, 0, nullptr, fixloc), what_register_enter.str());

    CHECK_THROWS_AS(GuardFW::io_uring_enter2(-1, 0, 0, 0, nullptr, 0, fixloc), std::system_error);
    CHECK_THROWS_WITH(GuardFW::io_uring_enter2(-1, 0, 0, 0, nullptr, 0, fixloc), what_register_enter.str());
}

TEST_CASE("io_uring working", "[io_uring]")
{
    struct io_uring_params params        = {};
    int fd_io_uring                      = -1;
    uint8_t* sq_ptr                      = nullptr;
    uint8_t* cq_ptr                      = nullptr;
    struct io_uring_sqe* sqes            = nullptr;
    struct io_uring_cqe* cqes            = nullptr;
    constexpr uint64_t user_test_pattern = 0x0123456789abcdefULL;

    // setup io_uring
    CHECK_NOTHROW((fd_io_uring = GuardFW::io_uring_setup(256, &params)));

    CHECK((params.features & IORING_FEAT_SINGLE_MMAP) != 0);  // minimum Linux 5.4

    constexpr unsigned int probe_nr = 256;
    constexpr size_t probe_size     = sizeof(struct io_uring_probe) + probe_nr * sizeof(struct io_uring_probe_op);
    struct io_uring_probe* probe    = reinterpret_cast<struct io_uring_probe*>(alloca(probe_size));
    memset(probe, 0, probe_size);

    CHECK_NOTHROW(GuardFW::io_uring_register(fd_io_uring, IORING_REGISTER_PROBE, probe, probe_nr, fixloc));

    size_t sring_sz = params.sq_off.array + (params.sq_entries * sizeof(unsigned));
    size_t cring_sz = params.cq_off.cqes + (params.cq_entries * sizeof(struct io_uring_cqe));

    sring_sz = cring_sz = std::max(cring_sz, sring_sz);

    CHECK_NOTHROW(
        (sq_ptr = reinterpret_cast<uint8_t*>(GuardFW::mmap(
             nullptr,
             sring_sz,
             GuardFW::constants::prot_read | GuardFW::constants::prot_write,
             GuardFW::constants::map_shared | GuardFW::constants::map_populate,
             fd_io_uring,
             IORING_OFF_SQ_RING
         )))
    );

    size_t sqes_size = params.sq_entries * sizeof(struct io_uring_sqe);
    CHECK_NOTHROW(
        (sqes = reinterpret_cast<struct io_uring_sqe*>(GuardFW::mmap(
             nullptr,
             sqes_size,
             GuardFW::constants::prot_read | GuardFW::constants::prot_write,
             GuardFW::constants::map_shared | GuardFW::constants::map_populate,
             fd_io_uring,
             IORING_OFF_SQES
         )))
    );

    cq_ptr = sq_ptr;

    std::atomic_ref<uint32_t> sring_head(*reinterpret_cast<uint32_t*>(sq_ptr + params.sq_off.head));
    std::atomic_ref<uint32_t> sring_tail(*reinterpret_cast<uint32_t*>(sq_ptr + params.sq_off.tail));
    uint32_t sring_mask   = *reinterpret_cast<uint32_t*>(sq_ptr + params.sq_off.ring_mask);
    uint32_t* sring_array = reinterpret_cast<uint32_t*>(sq_ptr + params.sq_off.array);

    std::atomic_ref<uint32_t> cring_head(*reinterpret_cast<uint32_t*>(cq_ptr + params.cq_off.head));
    std::atomic_ref<uint32_t> cring_tail(*reinterpret_cast<uint32_t*>(cq_ptr + params.cq_off.tail));
    uint32_t cring_mask = *reinterpret_cast<uint32_t*>(cq_ptr + params.cq_off.ring_mask);

    cqes = reinterpret_cast<struct io_uring_cqe*>(cq_ptr + params.cq_off.cqes);

    // insert nop command in submission queue
    uint32_t tail            = sring_tail.load(std::memory_order_consume);
    uint32_t index           = tail & sring_mask;
    struct io_uring_sqe* sqe = &sqes[index];

    sqe->opcode    = IORING_OP_NOP;
    sqe->fd        = -1;
    sqe->addr      = 0;
    sqe->len       = 0;
    sqe->off       = 0;
    sqe->user_data = user_test_pattern;

    sring_array[index] = index;
    sring_tail.store(tail + 1, std::memory_order_release);

    std::expected<unsigned int, int> entered;

    // blocks until nop event has completed
    CHECK_NOTHROW((entered = GuardFW::io_uring_enter(fd_io_uring, 1, 1, IORING_ENTER_GETEVENTS, nullptr)));
    CHECK(entered.has_value() == true);  // check that io_uring_enter succeeded

    // read completion queue
    uint32_t head = cring_head.load(std::memory_order_consume);
    CHECK(head != cring_tail.load(std::memory_order_relaxed));  // check that buffer is not empty

    struct io_uring_cqe* cqe = &cqes[head & cring_mask];
    CHECK(cqe->res >= 0);  // nop shall always succeed
    CHECK(cqe->user_data == user_test_pattern);
    cring_head.store(head + 1, std::memory_order_release);

    CHECK_NOTHROW(GuardFW::munmap(sq_ptr, sring_sz));
    CHECK_NOTHROW(GuardFW::munmap(sqes, sqes_size));

    CHECK_NOTHROW(GuardFW::close(fd_io_uring));
}
