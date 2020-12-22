#pragma once

#include <boost/algorithm/hex.hpp>
#include <boost/crc.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <iostream>
#include <list>
#include <vector>

using Path = boost::filesystem::path;
using ListPaths = std::list<Path>;
using ListRegexes = std::list<boost::regex>;
using ListStrings = std::list<std::string>;

struct Comparator {
  enum class HashMethod { md5, crc32 };

  /**
    @param path - путь к файлу.
    @param hashMethod - Метод хеширования.
   */
  Comparator(Path path, HashMethod hashMethod);

  /**
    Сравнить два файла.
    @param other - файл для сравнения.
    @return Совпадают ли файлы.
   */
  bool eq(std::unique_ptr<Comparator> &other);

  Path getFilePath() const;

private:
  /**
    Рассчитать хеш для диапазона блоков [0, blockIndex].
    @param blockIndex - Индекс блока, оканчивающего рассчёт.
   */
  void calculateHashForRange(size_t blockIndex);
  size_t calculateHash(std::string &data, HashMethod hashMethod);

  /**
    Рассчитать хеш для блока blockIndex.
    @param blockIndex - Индекс блока.
    @return Хеш блока.
   */
  size_t getHash(size_t blockIndex);

  uintmax_t mBlockSize = 512;
  Path mFilePath;
  uintmax_t mFileSize;
  size_t mMaxBlocksCount;
  HashMethod mHashMethod;
  std::vector<size_t> mHashs;
};
