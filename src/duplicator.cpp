#include "duplicator.hpp"

Duplicator::Duplicator(ProgramOptions::Parameters &parameters)
    : mByHash(parameters.byHash), mByMeta(!parameters.byHash),
      mDepth(parameters.depth), mMinimalSize(parameters.minimalSize),
      mHashMethod(parameters.hashMethod) {
  mIncludeDirs = setDirs(parameters.inDirs);
  mExcludeDirs = setDirs(parameters.exDirs);

  {
    ListRegexes temp;
    for (auto &e : parameters.inRegex)
      temp.emplace_back(boost::regex(e));
    mIncludeMasks = temp;
  }
  {
    ListRegexes temp;
    for (auto &e : parameters.exRegex)
      temp.emplace_back(boost::regex(e));
    mExcludeMasks = temp;
  }
}

Duplicates Duplicator::search() {
  for (auto &dir : mIncludeDirs) {
    for (auto &entry : boost::filesystem::directory_iterator(dir)) {
      scanDirectories(entry.path(), mDepth);
    }
  }

  std::vector<uintmax_t> keys;
  for (auto &pair : mScanFiles)
    if (std::find(keys.begin(), keys.end(), pair.first) == keys.end()) {
      keys.emplace_back(pair.first);
    }

  for (auto &key : keys) {
    findDuplicates(mScanFiles[key]);
  }

  for (auto &duplicate : mDuplicates) {
    std::cout << duplicate.first << std::endl;
    for (auto &path : duplicate.second) {
      std::cout << path << std::endl;
    }
    std::cout << std::endl;
  }

  return mDuplicates;
}

ListPaths Duplicator::setDirs(const ListStrings &dirs) {
  using namespace boost::filesystem;

  ListPaths directories;
  for (auto &dir : dirs) {
    auto path = weakly_canonical(boost::filesystem::absolute(dir));

    if (!exists(path)) {
      throw std::runtime_error("Directory doesn't exists: " + dir);
    }
    if (!is_directory(path)) {
      throw std::runtime_error("Path isn't a directory: " + dir);
    }
    directories.push_back(path);
  }
  return directories;
}

void Duplicator::findDuplicates(
    std::vector<std::unique_ptr<Comparator>> &files) {
  if (files.size() < 2) {
    return;
  }

  for (auto &first : files) {
    if (isAlreadyInDuplicates(first->getFilePath(), mDuplicates)) {
      continue;
    }
    for (auto &second : files) {
      if (isAlreadyInDuplicates(second->getFilePath(), mDuplicates) ||
          first->getFilePath() == second->getFilePath()) {
        continue;
      }

      bool filesEqual = true;

      if (mByHash) {
        filesEqual = filesEqual && first->eq(second);
      }
      if (mByMeta) {
        filesEqual = filesEqual && (first->getFilePath().filename().string() ==
                                    second->getFilePath().filename().string());
      }

      if (filesEqual)
        mDuplicates[first->getFilePath().string()].insert(
            second->getFilePath().string());
    }
  }
}

void Duplicator::scanDirectories(const Path &path, size_t depth) {
  using namespace boost::filesystem;

  if (exists(path) && !isExcludedDirs(path)) {
    if (is_regular_file(path)) {
      includeFile(path);
    } else if (is_directory(path) && depth) {
      for (const auto &dir : directory_iterator(path)) {
        scanDirectories(dir.path(), depth - 1);
      }
    }
  }
}

void Duplicator::includeFile(const Path &path) {
  using namespace boost::filesystem;

  if (!exists(path) || !is_regular_file(path) || file_size(path) < mMinimalSize)
    return;

  if ((!isFiltersSatisfied(path, mIncludeMasks) && !mIncludeMasks.empty()) ||
      (isFiltersSatisfied(path, mExcludeMasks) && !mExcludeMasks.empty()))
    return;

  std::string absolutePath = weakly_canonical(absolute(path)).string();
  if (mScanPaths.find(absolutePath) == mScanPaths.end()) {
    mScanPaths.insert(absolutePath);
    mScanFiles[file_size(path)].emplace_back(
        std::unique_ptr<Comparator>(new Comparator(absolutePath, mHashMethod)));
  }
}

bool Duplicator::isExcludedDirs(const Path &path) {
  using namespace boost::filesystem;

  for (const auto &dir : mExcludeDirs) {
    if (equivalent(path, dir)) {
      return true;
    }
  }

  return false;
}

bool Duplicator::isFiltersSatisfied(const Path &path,
                                    const ListRegexes &masks) {
  std::string fileName = path.filename().string();
  for (const auto &mask : masks)
    if (boost::regex_match(fileName, mask))
      return true;

  return false;
}

bool Duplicator::isAlreadyInDuplicates(const Path &path,
                                       Duplicates &duplicates) {
  bool already = false;
  auto &filepath = path.string();
  for (const auto &duplicate : duplicates) {
    if (duplicate.second.find(filepath) != duplicate.second.end()) {
      already = true;
      break;
    }
  }

  return already;
}
