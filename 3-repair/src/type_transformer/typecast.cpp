#include "type_transformer.h"

void TypeTransformer::transform_typecast(void) {
    for (auto i: NodeQuery::querySubTree(m_project, V_SgCastExp)) {
        // skip cast if excluded
        if (m_excluded->count(i)) continue;

        // skip system
        auto cast = isSgCastExp(i);
        if (misc_utils::insideSystemHeader(cast)) continue;

        auto target = recursive_transform(
                cast->get_type(),
                SageInterface::getGlobalScope(cast));
        if (!target) continue;

        INFO_IF(true, "[transform] cast type: ") <<
            misc_utils::debug_info(cast) << std::endl;

        // transform cast type
        cast->set_type(target);
    }
}
