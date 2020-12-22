#pragma once

#include "comparator.hpp"
#include "program_options.hpp"

#include <string>
#include <unordered_map>
#include <unordered_set>

using Duplicates =
    std::unordered_map<std::string, std::unordered_set<std::string>>;
using Files =
    std::unordered_map<uintmax_t, std::vector<std::unique_ptr<Comparator>>>;

struct Duplicator {
  Duplicator(ProgramOptions::Parameters &parameters);

  /**
    Основная процедура поиска дубликатов.
    @return Список дубликатов.
   */
  Duplicates search();

private:
  /**
    Убедиться в существовании директорий.
    @param dirs - Список директорий.
    @throw std::runtime_error, если одна из директорий не существует.
    @return Список директорий как boost::filesystem::path.
   */
  ListPaths setDirs(const ListStrings &dirs);

  /**
    Выполнить поиск дубликатов в подготовленном списке.
    @param files - Список файлов.
   */
  void findDuplicates(std::vector<std::unique_ptr<Comparator>> &files);

  /**
    Включить файлы из дерева директорий.
    @param path - Путь для сканирования.
    @param depth - Глубина сканирования.
   */
  void scanDirectories(const Path &path, size_t depth);

  /**
    Включить файл в список проверки.
    @param path - Путь к файлу.
   */
  void includeFile(const Path &path);

  /**
    Сверить директорию со списком исключенний.
    @param path - Директория.
    @return true, если есть.
   */
  bool isExcludedDirs(const Path &path);

  /**
    Проверить соответствие имени файла маске из списка.
    @param path - Путь к файлу.
    @param masks - Маски.
    @return true, если есть соответствие.
   */
  bool isFiltersSatisfied(const Path &path, const ListRegexes &masks);

  /**
    Файл уже проверен?
    @param path - Путь к файлу для проверки.
    @param duplicates - Список дубликатов.
    @return true, если файл уже проверен на дублирование.
   */
  bool isAlreadyInDuplicates(const Path &path, Duplicates &duplicates);

  bool mByHash;
  bool mByMeta;

  ListPaths mIncludeDirs;
  ListPaths mExcludeDirs;
  ListRegexes mIncludeMasks;
  ListRegexes mExcludeMasks;
  const size_t mDepth;
  const uint64_t mMinimalSize;
  const Comparator::HashMethod mHashMethod;

  std::unordered_set<std::string> mScanPaths;

  Files mScanFiles;
  Duplicates mDuplicates;
};
