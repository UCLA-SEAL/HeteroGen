#include "type_transformer.h"

void TypeTransformer::transform_func_param(void) {
    // transform definitions
    for (auto func: ast_utils::all_used_func_def(m_project)) {
        for (auto param: func->get_parameterList()->get_args()) {
            // skip param if excluded
            if (m_excluded->count(param)) continue;

            // skip system
            if (misc_utils::insideSystemHeader(func)) continue;

            // try transform type
            auto type = recursive_transform(
                    param->get_type(),
                    SageInterface::getGlobalScope(param));

            // skip param if not transformed
            if (!type) continue;

            INFO_IF(true, "[transform] function definition param type: ") <<
                misc_utils::debug_info(param) << std::endl;

            // transform function param type
            param->set_type(type);
        }
    }
}
