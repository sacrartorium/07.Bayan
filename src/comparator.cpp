#include "comparator.hpp"

Comparator::Comparator(Path path, HashMethod hashMethod)
    : mFilePath(path), mFileSize(boost::filesystem::file_size(mFilePath)),
      mMaxBlocksCount((mFileSize + mBlockSize - 1) / mBlockSize),
      mHashMethod(hashMethod) {}

bool Comparator::eq(std::unique_ptr<Comparator> &other) {
  if (mFileSize != other->mFileSize) {
    return false;
  }

  for (size_t index = 0; index < mMaxBlocksCount; index++) {
    if (getHash(index) != other->getHash(index))
      return false;
  }

  return true;
}

Path Comparator::getFilePath() const { return mFilePath; }

std::string stringify(boost::uuids::detail::md5::digest_type &digest) {
  auto charDigest = reinterpret_cast<const char *>(&digest);
  std::string result;
  boost::algorithm::hex(
      charDigest, charDigest + sizeof(boost::uuids::detail::md5::digest_type),
      std::back_inserter(result));

  return result;
}

size_t Comparator::calculateHash(std::string &data, HashMethod hashMethod) {
  if (hashMethod == Comparator::HashMethod::md5) {
    boost::uuids::detail::md5 hashFn;
    hashFn.process_bytes(data.data(), data.size());

    boost::uuids::detail::md5::digest_type result;
    hashFn.get_digest(result);
    std::stringstream res(stringify(result));
    size_t hash;
    res >> hash;

    return hash;
  } else if (hashMethod == Comparator::HashMethod::crc32) {
    boost::crc_32_type hashFn;
    hashFn.process_bytes(data.data(), data.size());

    return hashFn.checksum();
  }

  return 0;
}

void Comparator::calculateHashForRange(size_t blockIndex) {
  while (mHashs.size() <= blockIndex) {
    std::string data(mBlockSize, '\0');

    std::ifstream file;
    file.open(mFilePath.string());
    file.seekg(mBlockSize * mHashs.size());
    file.read(&data[0], mBlockSize);
    file.close();

    mHashs.emplace_back(calculateHash(data, mHashMethod));
  }
}

size_t Comparator::getHash(size_t blockIndex) {
  if (blockIndex >= mMaxBlocksCount) {
    throw std::logic_error("Error block index!");
  }
  if (blockIndex < mHashs.size()) {
    return mHashs[blockIndex];
  }
  calculateHashForRange(blockIndex);

  return mHashs[blockIndex];
}
