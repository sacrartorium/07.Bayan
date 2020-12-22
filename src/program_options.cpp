#include "program_options.hpp"

ProgramOptions::ProgramOptions() : mDescription{"Options"} {}

OptionsDescription *ProgramOptions::operator->() { return &mDescription; }

const OptionsDescription *ProgramOptions::operator->() const {
  return &mDescription;
}

void ProgramOptions::parse(int argc, const char **argv) {
  try {
    Options::store(Options::parse_command_line(argc, argv, mDescription),
                   mOptions);
    Options::notify(mOptions);
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}

bool ProgramOptions::scan(Options::variables_map &options, const char *option,
                          std::function<void(const char *option)> action) {
  bool result = options.count(option);
  if (result)
    action(option);
  return result;
}

ProgramOptions::Parameters ProgramOptions::execute() {
  if (!scan(mOptions, "help", [this]([[maybe_unused]] const char *option) {
        mParameters.isHelped = true;
        std::cout << mDescription << std::endl;
      })) {

    scan(mOptions, "method", [this](const char *option) {
      auto method =
          boost::to_lower_copy<std::string>(mOptions[option].as<std::string>());
      if (method == "md5")
        mParameters.hashMethod = Comparator::HashMethod::md5;
      else if (method == "crc32")
        mParameters.hashMethod = Comparator::HashMethod::crc32;
      else
        mParameters.hashMethod = Comparator::HashMethod::md5;
    });

    scan(mOptions, "type", [this](const char *option) {
      auto type =
          boost::to_lower_copy<std::string>(mOptions[option].as<std::string>());
      if (type == "hash")
        mParameters.byHash = true;
      else if (type == "name")
        mParameters.byHash = false;
      else
        mParameters.byHash = true;
    });

    scan(mOptions, "depth", [this](const char *option) {
      mParameters.depth = mOptions[option].as<size_t>();
    });
    scan(mOptions, "msize", [this](const char *option) {
      mParameters.minimalSize = mOptions[option].as<size_t>();
    });
    scan(mOptions, "bsize", [this](const char *option) {
      mParameters.blockSize = mOptions[option].as<size_t>();
    });

    scan(mOptions, "indirs", [this](const char *option) {
      auto temp = mOptions[option].as<std::vector<std::string>>();
      mParameters.inDirs = ListStrings(temp.begin(), temp.end());
    });
    scan(mOptions, "exdirs", [this](const char *option) {
      auto temp = mOptions[option].as<std::vector<std::string>>();
      mParameters.exDirs = ListStrings(temp.begin(), temp.end());
    });
    scan(mOptions, "inregex", [this](const char *option) {
      auto temp = mOptions[option].as<std::vector<std::string>>();
      mParameters.inRegex = ListStrings(temp.begin(), temp.end());
    });
    scan(mOptions, "exregex", [this](const char *option) {
      auto temp = mOptions[option].as<std::vector<std::string>>();
      mParameters.exRegex = ListStrings(temp.begin(), temp.end());
    });
  }

  return mParameters;
}
