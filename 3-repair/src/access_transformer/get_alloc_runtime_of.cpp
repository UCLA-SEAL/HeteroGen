#include "access_transformer.h"
#include "resource.h"

#include <cstdio>
#include <cstdlib>

std::string AccessTransformer::get_alloc_runtime_of(
        std::string access_name, std::string type_str, int size_log2) {

    char tmpin[] = "/tmp/dst_tempin_XXXXXX";
    ::close(::mkstemp(tmpin));
    std::ofstream infile(tmpin);
    if (m_is_instrument)
        infile << resource::trace_alloc;
    else
        infile << resource::runtime_alloc;
    infile.close();

    char tmpout[] = "/tmp/dst_tempout_XXXXXX";
    ::close(::mkstemp(tmpout));
    std::system((std::string() + "cpp" +
                " -DELEMENT_LOG2=\"" + std::to_string(size_log2) +
                "\" -DACCESS_NAME=\"" + access_name +
                "\" -DACCESS_TYPE=\"" + type_str +
                "\" <" + tmpin + " >" + tmpout).c_str());

    auto stream = std::ifstream(tmpout);
    std::string s(std::istreambuf_iterator<char>(stream), {});

    std::remove(tmpin);
    std::remove(tmpout);

    return s;
}
