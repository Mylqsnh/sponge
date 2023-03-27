#include "wrapping_integers.hh"

#include <iostream>

// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;


/*int main () {
    cout << wrap(3 * (1ll << 32) + 17, WrappingInt32(15));
    cout << endl;

//    cout << unwrap(WrappingInt32(1), WrappingInt32(0), UINT32_MAX);
    cout << unwrap(WrappingInt32(0), WrappingInt32(INT32_MAX), 0);
    cout << endl;
    cout << wrap(2147483649, WrappingInt32(INT32_MAX));
    cout << endl;

}*/

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    //相对长度 = 绝对长度 + Initial Sequence Number(ISN)
    return WrappingInt32(static_cast<uint32_t>(n) + isn.raw_value());
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    //相对地址 - Initial Sequence Number(ISN)
    uint32_t offset = n.raw_value() - isn.raw_value();
    //取高32位
    uint64_t t = (checkpoint & 0xFFFFFFFF00000000) + offset;
    uint64_t ret = t;
    //取进值
    if (abs(int64_t(t + (1ul << 32) - checkpoint)) < abs(int64_t(t - checkpoint)))
        ret = t + (1ul << 32);
    if (t >= (1ul << 32) && abs(int64_t(t - (1ul << 32) - checkpoint)) < abs(int64_t(ret - checkpoint)))
        ret = t - (1ul << 32);
    return ret;
}
