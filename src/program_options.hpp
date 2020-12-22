#pragma once

#include "comparator.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

namespace Options = boost::program_options;
using OptionsDescription = boost::program_options::options_description;

struct ProgramOptions {

  /**
    Конструктор класса для поиска файлов-дубликатов
   */
  struct Parameters {
    bool byHash = false; /**< Поиск по хешу или имени. */
    Comparator::HashMethod hashMethod; /**< Метод хеширования. */
    size_t depth = 0; /**< Глубина сканироания директорий. */
    size_t minimalSize =
        1; /**< Минимальный размер сканируемого файла. (байты) */
    size_t blockSize = 512;
    ListStrings inDirs; /**< Директории для сканирования. */
    ListStrings exDirs; /**< Директории, исключенные из сканирования. */
    ListStrings inRegex; /**< Если список не пуст, то поиск производится только
                            среди удовлетворяющих маскам путей. */
    ListStrings exRegex; /**< Маски, для исключения файлов из поиска. */
    bool isHelped = false; /**< Режим помощи. Поиск не выполняется. */
  };

  ProgramOptions();

  OptionsDescription *operator->();
  const OptionsDescription *operator->() const;

  void parse(int argc, const char **argv);
  Parameters execute();

  Parameters mParameters;

private:
  bool scan(Options::variables_map &options, const char *option,
            std::function<void(const char *option)> action);

  Options::variables_map mOptions;
  OptionsDescription mDescription;
};
