#include "type_transformer.h"

void TypeTransformer::transform_var_decl(void) {
    for (auto var: ast_utils::all_used_var_decl(m_project)) {
        // skip var if excluded
        if (m_excluded->count(var)) continue;

        // skip system
        if (misc_utils::insideSystemHeader(var)) continue;

        // try transform type
        SgType  *type;

        if (isSgType(var->get_type())->isIntegerType()) {
            type = recursive_transform_i(
                var->get_type(),
                SageInterface::getGlobalScope(var),var->get_name());
        } else{
            type = recursive_transform(
                var->get_type(),
                SageInterface::getGlobalScope(var));
        }

        // skip var if not transformed
        if (!type) continue;

        INFO_IF(true, "[transform] variable type: ") <<
            misc_utils::debug_info(var) << std::endl;

        // transform variable type
        var->set_type(type);
    }
}
