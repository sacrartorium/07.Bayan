#include "duplicator.hpp"
#include "program_options.hpp"

void run(std::unique_ptr<ProgramOptions> &po) {
  auto options = po->execute();
  if (!options.isHelped) {
    Duplicator duplicator(options);

    duplicator.search();
  }
}

int main(int argc, const char **argv) {
  try {
    std::unique_ptr<ProgramOptions> po = std::make_unique<ProgramOptions>();

    po->operator->()->add_options()(
        "method,o", Options::value<std::string>()->default_value("md5"),
        "Select hash function. (md5, crc32)");
    po->operator->()->add_options()(
        "type,t", Options::value<std::string>()->default_value("hash"),
        "Select search type. (hash, name)");
    po->operator->()->add_options()("indirs,i",
                                    Options::value<std::vector<std::string>>()
                                        ->multitoken()
                                        ->zero_tokens()
                                        ->composing(),
                                    "Include directories to find.");
    po->operator->()->add_options()("exdirs,e",
                                    Options::value<std::vector<std::string>>()
                                        ->multitoken()
                                        ->zero_tokens()
                                        ->composing(),
                                    "Exclude directories to find.");
    po->operator->()->add_options()("inregex,n",
                                    Options::value<std::vector<std::string>>()
                                        ->multitoken()
                                        ->zero_tokens()
                                        ->composing(),
                                    "Include regex mask to find.");
    po->operator->()->add_options()("exregex,x",
                                    Options::value<std::vector<std::string>>()
                                        ->multitoken()
                                        ->zero_tokens()
                                        ->composing(),
                                    "Exclude regex mask to find.");

    po->operator->()->add_options()("msize,m",
                                    Options::value<size_t>()->default_value(1),
                                    "Minimum file size (in bytes) to find.");
    po->operator->()->add_options()(
        "bsize,b", Options::value<size_t>()->default_value(512),
        "Size of block to hash.");
    po->operator->()->add_options()(
        "depth,d", Options::value<size_t>()->default_value(0),
        "Depth of scan. (0 - only this level, 1 - all nested levels)");

    po->operator->()->add_options()("help,h", "Show help.");
    po->parse(argc, argv);

    run(po);

  } catch (std::exception &e) {
    std::cerr << "Error! " << e.what() << std::endl;
  }
  return 0;
}
