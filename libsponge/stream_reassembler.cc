#include "stream_reassembler.hh"

#include <iostream>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {
    _buffer.resize(capacity);
}

long StreamReassembler::merge_block(block_node &elm1, const block_node &elm2) {
    block_node x, y;
    // x 为小索引节点， y为大索引节点
    if (elm1.begin > elm2.begin) {
        x = elm2;
        y = elm1;
    } else {
        x = elm1;
        y = elm2;
    }
    // 两节点不相邻，无法合并
    if (x.begin + x.length < y.begin) {
        return -1;  // no intersection, couldn't merge
    }
    // 包含关系，取小索引节点
    else if (x.begin + x.length >= y.begin + y.length) {
        elm1 = x;
        return y.length;
    }
    // 合并有效字符
    else {
        elm1.begin = x.begin;
        elm1.data = x.data + y.data.substr(x.begin + x.length - y.begin);
        elm1.length = elm1.data.length();
        return x.begin + x.length - y.begin;
    }
}

/*int main() {
    size_t a = 10;
    StreamReassembler streamReassembler(a);
    streamReassembler.push_substring("asdzxc", 0, 0);
    streamReassembler.push_substring("cqwe", 5, 0);
    streamReassembler.push_substring("asd", 9, 1);

    size_t size = streamReassembler.stream_out().buffer_size();

    cout << streamReassembler.stream_out().peek_output(size);

}*/

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // ALL = ( _head_index = stream start ) + (_capacity = ( first unread + first unassembled ) )
    // 索引值超过总容量
    if (index >= _head_index + _capacity) {  // capacity over
        return;
    }

    // handle extra substring prefix
    block_node elm;
    // 索引值 + 数据长度 <= 已读取长度
    if (index + data.length() <= _head_index) {  // couldn't equal, because there have emtpy substring
        //该数据已经被读取过，不再处理
        goto JUDGE_EOF;
    }
    // 包含部分已经被读取的数据，截取有效位置
    else if (index < _head_index) {
        size_t offset = _head_index - index;
        elm.data.assign(data.begin() + offset, data.end());
        elm.begin = index + offset;
        elm.length = elm.data.length();
    } else {
        elm.begin = index;
        elm.length = data.length();
        elm.data = data;
    }
    _unassembled_byte += elm.length;

    // merge substring
    // _blocks<set> 节点有序
    do {
        // merge next
        long merged_bytes = 0;
        // 重载运算符 < 寻找索引小于当前节点位置的节点
        // lower_bound : 二分查找第一个小于或等于num的数字
        auto iter = _blocks.lower_bound(elm);
        // 合并
        while (iter != _blocks.end() && (merged_bytes = merge_block(elm, *iter)) >= 0) {
            // 减少无序字符
            _unassembled_byte -= merged_bytes;
            // 移除已排序节点
            _blocks.erase(iter);
            // next
            iter = _blocks.lower_bound(elm);
        }
        // merge prev
        //继续向前合并
        if (iter == _blocks.begin()) {
            break;
        }
        iter--;
        while ((merged_bytes = merge_block(elm, *iter)) >= 0) {
            _unassembled_byte -= merged_bytes;
            _blocks.erase(iter);
            iter = _blocks.lower_bound(elm);
            if (iter == _blocks.begin()) {
                break;
            }
            iter--;
        }
    } while (false);
    _blocks.insert(elm);

    // write to ByteStream
    if (!_blocks.empty() && _blocks.begin()->begin == _head_index) {
        const block_node head_block = *_blocks.begin();
        // modify _head_index and _unassembled_byte according to successful write to _output
        size_t write_bytes = _output.write(head_block.data);
        // 从零计数，期望值 
        _head_index += write_bytes;
        _unassembled_byte -= write_bytes;
        _blocks.erase(_blocks.begin());
    }

JUDGE_EOF:
    if (eof) {
        _eof_flag = true;
    }
    if (_eof_flag && empty()) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_byte; }

bool StreamReassembler::empty() const { return _unassembled_byte == 0; }
