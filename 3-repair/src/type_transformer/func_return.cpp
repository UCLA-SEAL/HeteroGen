#include "type_transformer.h"

void TypeTransformer::transform_func_return(void) {
    for (auto func: ast_utils::all_used_func_def(m_project)) {
        // skip function if it is excluded
        if (m_excluded->count(func)) continue;

        // skip system
        if (misc_utils::insideSystemHeader(func)) continue;

        auto target = recursive_transform(
                func->get_type()->get_return_type(),
                SageInterface::getGlobalScope(func));
        if (!target) continue;

        INFO_IF(true, "[transform] function return type: ") <<
            misc_utils::debug_info(func) << std::endl;

        // transform function return type
        func->set_type(SageBuilder::buildFunctionType(target,
                    func->get_type()->get_argument_list()));
    }
}
