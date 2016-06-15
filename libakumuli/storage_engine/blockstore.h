#pragma once
#include "volume.h"
#include <random>

namespace Akumuli {
namespace StorageEngine {

//! Address of the block inside storage
typedef u64 LogicAddr;


class Block;

struct BlockCache {
    typedef std::shared_ptr<Block> PBlock;
    std::vector<PBlock> block_cache_;
    const u32 bits_;
    // RNG
    std::random_device dev_;
    std::mt19937 gen_;
    std::uniform_int_distribution<u32> dist_;

    /** Check status of the cache cell.
      * Return 0 if there is no such addr in the cache and slot is free.
      * Return 1 if addr is not present in the cache but slot is occupied by the other block.
      * Return 2 if addr is already present in the cache.
      */
    int probe(LogicAddr addr);

    BlockCache(u32 Nbits);

    void insert(PBlock block);

    PBlock loockup(LogicAddr addr);
};

struct BlockStore {

    virtual ~BlockStore() = default;

    /** Read block from blockstore
      */
    virtual std::tuple<aku_Status, std::shared_ptr<Block>> read_block(LogicAddr addr) = 0;

    /** Add block to blockstore.
      * @param data Pointer to buffer.
      * @return Status and block's logic address.
      */
    virtual std::tuple<aku_Status, LogicAddr> append_block(u8 const* data) = 0;

    //! Flush all pending changes.
    virtual void flush() = 0;

    //! Check if addr exists in block-store
    virtual bool exists(LogicAddr addr) const = 0;

    //! Compute checksum of the input data.
    virtual u32 checksum(u8 const* begin, size_t size) const = 0;
};

/** Blockstore. Contains collection of volumes.
  * Translates logic adresses into physical ones.
  */
class FixedSizeFileStorage : public BlockStore,
                             public std::enable_shared_from_this<FixedSizeFileStorage> {
    //! Metadata volume.
    std::unique_ptr<MetaVolume> meta_;
    //! Array of volumes.
    std::vector<std::unique_ptr<Volume>> volumes_;
    //! "Dirty" flags.
    std::vector<int> dirty_;
    //! Current volume.
    u32 current_volume_;
    //! Current generation.
    u32 current_gen_;
    //! Size of the blockstore in blocks.
    size_t total_size_;

    //! Secret c-tor.
    FixedSizeFileStorage(std::string metapath, std::vector<std::string> volpaths);

    void advance_volume();

public:
    /** Create BlockStore instance (can be created only on heap).
      */
    static std::shared_ptr<FixedSizeFileStorage> open(std::string              metapath,
                                                      std::vector<std::string> volpaths);

    static void create(std::string metapath, std::vector<std::tuple<u32, std::string>> vols);

    /** Read block from blockstore
      */
    virtual std::tuple<aku_Status, std::shared_ptr<Block>> read_block(LogicAddr addr);

    /** Add block to blockstore.
      * @param data Pointer to buffer.
      * @return Status and block's logic address.
      */
    virtual std::tuple<aku_Status, LogicAddr> append_block(u8 const* data);

    virtual void flush();

    virtual bool exists(LogicAddr addr) const;

    virtual u32 checksum(u8 const* data, size_t size) const;
};

//! Represents memory block
class Block {
    std::weak_ptr<BlockStore> store_;
    std::vector<u8>           data_;
    LogicAddr                 addr_;

public:
    Block(std::shared_ptr<BlockStore> bs, LogicAddr addr, std::vector<u8>&& data);

    const u8* get_data() const;

    size_t get_size() const;

    LogicAddr get_addr() const;
};

//! Should be used to create blockstore
struct BlockStoreBuilder {
    static std::shared_ptr<BlockStore> create_memstore();
    static std::shared_ptr<BlockStore> create_memstore(std::function<void(LogicAddr)> append_cb);
};

}
}  // namespace
