// copyright defined in LICENSE.txt

// todo: first vs. first_key, last vs. last_key
// todo: results vs. response vs. rows

#pragma once
#include <eosio/struct-reflection.hpp>
#include <eosio/temp-placeholders.hpp>
#include <eosiolib/time.hpp>

namespace eosio {

/// \group increment_key Increment Key
/// Increment a database key. Return true if the result wrapped.
inline bool increment_key(uint8_t& key) { return !++key; }

/// \group increment_key
inline bool increment_key(uint16_t& key) { return !++key; }

/// \group increment_key
inline bool increment_key(uint32_t& key) { return !++key; }

/// \group increment_key
inline bool increment_key(uint64_t& key) { return !++key; }

/// \group increment_key
inline bool increment_key(uint128_t& key) { return !++key; }

/// \group increment_key
inline bool increment_key(name& key) { return !++key.value; }

/// \group increment_key
inline bool increment_key(checksum256& key) { return increment_key(key.data()[1]) && increment_key(key.data()[0]); }

/// \output_section Tables
/// Transaction status
enum class transaction_status : uint8_t {
    /// succeed, no error handler executed
    executed = 0,
    /// objectively failed (not executed), error handler executed
    soft_fail = 1,
    /// objectively failed and error handler objectively failed thus no state change
    hard_fail = 2,
    /// transaction delayed/deferred/scheduled for future execution
    delayed = 3,
    /// transaction expired and storage space refunded to user
    expired = 4,
};

/// Information extracted from a block
struct block_info {
    uint32_t                    block_num             = {};
    serial_wrapper<checksum256> block_id              = {};
    block_timestamp             timestamp             = block_timestamp{};
    name                        producer              = {};
    uint16_t                    confirmed             = {};
    serial_wrapper<checksum256> previous              = {};
    serial_wrapper<checksum256> transaction_mroot     = {};
    serial_wrapper<checksum256> action_mroot          = {};
    uint32_t                    schedule_version      = {};
    uint32_t                    new_producers_version = {};
    //std::vector<producer_key> new_producers         = {}; // todo
};

/// \exclude
inline std::string_view schema_type_name(block_info*) { return "eosio::block_info"; }

/// \exclude
template <typename F>
void for_each_member(block_info*, F f) {
    STRUCT_MEMBER(block_info, block_num)
    STRUCT_MEMBER(block_info, block_id)
    STRUCT_MEMBER(block_info, timestamp)
    STRUCT_MEMBER(block_info, producer)
    STRUCT_MEMBER(block_info, confirmed)
    STRUCT_MEMBER(block_info, previous)
    STRUCT_MEMBER(block_info, transaction_mroot)
    STRUCT_MEMBER(block_info, action_mroot)
    STRUCT_MEMBER(block_info, schedule_version)
    STRUCT_MEMBER(block_info, new_producers_version)
}

/// Details about action execution
struct action_trace {
    uint32_t                    block_index             = {};
    serial_wrapper<checksum256> transaction_id          = {};
    uint32_t                    action_index            = {};
    uint32_t                    parent_action_index     = {};
    eosio::transaction_status   transaction_status      = {};
    eosio::name                 receipt_receiver        = {};
    serial_wrapper<checksum256> receipt_act_digest      = {};
    uint64_t                    receipt_global_sequence = {};
    uint64_t                    receipt_recv_sequence   = {};
    unsigned_int                receipt_code_sequence   = {};
    unsigned_int                receipt_abi_sequence    = {};
    eosio::name                 account                 = {};
    eosio::name                 name                    = {};
    datastream<const char*>     data                    = {nullptr, 0};
    bool                        context_free            = {};
    int64_t                     elapsed                 = {};

    EOSLIB_SERIALIZE(
        action_trace, (block_index)(transaction_id)(action_index)(parent_action_index)(transaction_status)(receipt_receiver)(
                          receipt_act_digest)(receipt_global_sequence)(receipt_recv_sequence)(receipt_code_sequence)(receipt_abi_sequence)(
                          account)(name)(data)(context_free)(elapsed))
};

/// \exclude
inline std::string_view schema_type_name(action_trace*) { return "eosio::action_trace"; }

/// Details about an account
struct account {
    uint32_t                    block_index      = {};
    bool                        present          = {};
    eosio::name                 name             = {};
    uint8_t                     vm_type          = {};
    uint8_t                     vm_version       = {};
    bool                        privileged       = {};
    time_point                  last_code_update = time_point{};
    serial_wrapper<checksum256> code_version     = {};
    block_timestamp_type        creation_date    = block_timestamp_type{};
    datastream<const char*>     code             = {nullptr, 0};
    datastream<const char*>     abi              = {nullptr, 0};

    EOSLIB_SERIALIZE(
        account, (block_index)(present)(name)(vm_type)(vm_version)(privileged)(last_code_update)(code_version)(creation_date)(code)(abi))
};

/// \exclude
inline std::string_view schema_type_name(account*) { return "eosio::account"; }

/// \exclude
template <typename F>
void for_each_member(account*, F f) {
    STRUCT_MEMBER(account, block_index)
    STRUCT_MEMBER(account, present)
    STRUCT_MEMBER(account, name)
    STRUCT_MEMBER(account, vm_type)
    STRUCT_MEMBER(account, vm_version)
    STRUCT_MEMBER(account, privileged)
    STRUCT_MEMBER(account, last_code_update)
    STRUCT_MEMBER(account, code_version)
    STRUCT_MEMBER(account, creation_date)
    STRUCT_MEMBER(account, code)
    STRUCT_MEMBER(account, abi)
}

/// A row in a contract's table
struct contract_row {
    uint32_t                block_index = {};
    bool                    present     = {};
    name                    code        = {};
    uint64_t                scope       = {};
    name                    table       = {};
    uint64_t                primary_key = {};
    name                    payer       = {};
    datastream<const char*> value       = {nullptr, 0};
};

/// \exclude
inline std::string_view schema_type_name(contract_row*) { return "eosio::contract_row"; }

/// A secondary index entry in a contract's table. Also includes fields from `contract_row`.
template <typename T>
struct contract_secondary_index_with_row {
    uint32_t                block_index     = {};
    bool                    present         = {};
    name                    code            = {};
    uint64_t                scope           = {};
    name                    table           = {};
    uint64_t                primary_key     = {};
    name                    payer           = {};
    T                       secondary_key   = {};
    uint32_t                row_block_index = {};
    bool                    row_present     = {};
    name                    row_payer       = {};
    datastream<const char*> row_value       = {nullptr, 0};
};

/// \output_section Queries
/// Get `block_info` for a range of block indexes.
struct query_block_info_range_index {
    name query_name = "block.info"_n;

    /// First block index to retrieve
    uint32_t first = {};

    /// Last block index to retrieve
    uint32_t last = {};

    /// Maximum results to return. The wasm-ql server may cap the number of results to a smaller number.
    uint32_t max_results = {};
};

/// Get `action_trace` for a range of keys. Only includes actions in executed transactions.
///
/// The order of fields in `key` defines the order of query results. Every record has a different key.
/// ```c++
/// struct key {
///     eosio::name                 name             = {};
///     eosio::name                 receipt_receiver = {};
///     eosio::name                 account          = {};
///     uint32_t                    block_index      = {};
///     serial_wrapper<checksum256> transaction_id   = {};
///     uint32_t                    action_index     = {};
///
///     // Construct the key from `data`
///     static key from_data(const action_trace& data);
/// };
/// ```
struct query_action_trace_executed_range_name_receiver_account_block_trans_action {
    struct key {
        eosio::name                 name             = {};
        eosio::name                 receipt_receiver = {};
        eosio::name                 account          = {};
        uint32_t                    block_index      = {};
        serial_wrapper<checksum256> transaction_id   = {};
        uint32_t                    action_index     = {};

        // Extract the key from `data`
        static key from_data(const action_trace& data) {
            return {
                .name             = data.name,
                .receipt_receiver = data.receipt_receiver,
                .account          = data.account,
                .block_index      = data.block_index,
                .transaction_id   = data.transaction_id,
                .action_index     = data.action_index,
            };
        }
    };

    name query_name = "at.e.nra"_n;

    /// Look at this point of time in the history
    uint32_t max_block = {};

    /// Query records with keys in the range [`first`, `last`].
    key first = {};

    /// Query records with keys in the range [`first`, `last`].
    key last = {};

    /// Maximum results to return. The wasm-ql server may cap the number of results to a smaller number.
    uint32_t max_results = {};
};

/// \group increment_key
inline bool increment_key(query_action_trace_executed_range_name_receiver_account_block_trans_action::key& key) {
    return increment_key(key.action_index) &&         //
           increment_key(key.transaction_id.value) && //
           increment_key(key.block_index) &&          //
           increment_key(key.account) &&              //
           increment_key(key.receipt_receiver) &&     //
           increment_key(key.name);
}

struct query_account_range_name {
    name     query_name  = "account"_n;
    uint32_t max_block   = {};
    name     first       = {};
    name     last        = {};
    uint32_t max_results = {};
};

struct query_contract_row_range_code_table_pk_scope {
    // todo: from_data, increment_key
    struct key {
        name     code        = {};
        name     table       = {};
        uint64_t primary_key = {};
        uint64_t scope       = {};
    };

    name     query_name  = "cr.ctps"_n;
    uint32_t max_block   = {};
    key      first       = {};
    key      last        = {};
    uint32_t max_results = {};
};

struct query_contract_row_range_code_table_scope_pk {
    // todo: from_data, increment_key
    struct key {
        name     code        = {};
        name     table       = {};
        uint64_t scope       = {};
        uint64_t primary_key = {};
    };

    name     query_name  = "cr.ctsp"_n;
    uint32_t max_block   = {};
    key      first       = {};
    key      last        = {};
    uint32_t max_results = {};
};

struct query_contract_row_range_scope_table_pk_code {
    // todo: from_data, increment_key
    struct key {
        uint64_t scope       = {};
        name     table       = {};
        uint64_t primary_key = {};
        name     code        = {};
    };

    name     query_name  = "cr.stpc"_n;
    uint32_t max_block   = {};
    key      first       = {};
    key      last        = {};
    uint32_t max_results = {};
};

struct query_contract_index64_range_code_table_scope_sk_pk {
    // todo: from_data, increment_key
    struct key {
        name     code          = {};
        name     table         = {};
        uint64_t scope         = {};
        uint64_t secondary_key = {};
        uint64_t primary_key   = {};
    };

    name     query_name  = "ci1.cts2p"_n;
    uint32_t max_block   = {};
    key      first       = {};
    key      last        = {};
    uint32_t max_results = {};
};

/// \output_section Database Status
/// Status of the database. Returned by `get_database_status`.
struct database_status {
    uint32_t    head            = {};
    checksum256 head_id         = {};
    uint32_t    irreversible    = {};
    checksum256 irreversible_id = {};
    uint32_t    first           = {};
};

/// \exclude
inline std::string_view schema_type_name(database_status*) { return "eosio::database_status"; }

/// \exclude
template <typename F>
void for_each_member(database_status*, F f) {
    STRUCT_MEMBER(database_status, head)
    STRUCT_MEMBER(database_status, head_id)
    STRUCT_MEMBER(database_status, irreversible)
    STRUCT_MEMBER(database_status, irreversible_id)
    STRUCT_MEMBER(database_status, first)
}

// todo: version number argument
/// \exclude
extern "C" void get_database_status(void* cb_alloc_data, void* (*cb_alloc)(void* cb_alloc_data, size_t size));

/// \exclude
template <typename Alloc_fn>
inline void get_database_status(Alloc_fn alloc_fn) {
    get_database_status(&alloc_fn, [](void* cb_alloc_data, size_t size) -> void* { //
        return (*reinterpret_cast<Alloc_fn*>(cb_alloc_data))(size);
    });
}

/// Get the current database status
inline database_status get_database_status() {
    database_status   result;
    std::vector<char> bin;
    get_database_status([&bin](size_t size) {
        bin.resize(size);
        return bin.data();
    });
    datastream<const char*> ds(bin.data(), bin.size());
    ds >> result;
    return result;
}

/// \output_section Query Database
/// Query the database. `request` must be one of the `query_*` structs. Returns result in serialized form.
/// The serialized form is the same as `vector<vector<char>>`'s serialized form. Each inner vector contains the
/// serialized form of a row. The row type varies with query.
///
/// Use `for_each_query_result` or `for_each_contract_row` to iterate through the result.
template <typename T>
inline std::vector<char> query_database(const T& request) {
    std::vector<char> result;
    query_database(request, [&result](size_t size) {
        result.resize(size);
        return result.data();
    });
    return result;
}

/// Unpack each row of a query result and call `f(row)`. `T` is the row type.
template <typename T, typename F>
bool for_each_query_result(const std::vector<char>& bytes, F f) {
    datastream<const char*> ds(bytes.data(), bytes.size());
    unsigned_int            size;
    ds >> size;
    for (uint32_t i = 0; i < size.value; ++i) {
        datastream<const char*> row{nullptr, 0};
        ds >> row;
        T r;
        row >> r;
        if (!f(r))
            return false;
    }
    return true;
}

/// Use with `query_contract_row_*`. Unpack each row of a query result and call
/// `f(row, data)`. `row` is an instance of `contract_row`. `data` is the unpacked
/// contract-specific data. `T` identifies the type of `data`.
template <typename T, typename F>
bool for_each_contract_row(const std::vector<char>& bytes, F f) {
    return for_each_query_result<contract_row>(bytes, [&](contract_row& row) {
        T p;
        if (row.present && row.value.remaining()) {
            // todo: don't assert if serialization fails
            row.value >> p;
            if (!f(row, &p))
                return false;
        } else {
            if (!f(row, nullptr))
                return false;
        }
        return true;
    });
}

/// \exclude
extern "C" void query_database(void* req_begin, void* req_end, void* cb_alloc_data, void* (*cb_alloc)(void* cb_alloc_data, size_t size));

/// \exclude
template <typename T, typename Alloc_fn>
inline void query_database(const T& req, Alloc_fn alloc_fn) {
    auto req_data = pack(req);
    query_database(req_data.data(), req_data.data() + req_data.size(), &alloc_fn, [](void* cb_alloc_data, size_t size) -> void* {
        return (*reinterpret_cast<Alloc_fn*>(cb_alloc_data))(size);
    });
}

} // namespace eosio