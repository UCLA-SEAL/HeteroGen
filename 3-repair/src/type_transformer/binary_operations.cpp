#include "type_transformer.h"

void TypeTransformer::transform_binary_operations(void) {
    if (m_type != misc_utils::RefactorType::fp) return;

    for (auto i: NodeQuery::querySubTree(m_project, V_SgBinaryOp)) {
        auto op = isSgBinaryOp(i);
        if (misc_utils::insideSystemHeader(op)) continue;

        // transform float to fp type
        if (op->get_lhs_operand()->get_type()->isFloatType() &&
                op->get_lhs_operand()->get_lvalue() == false) {
            INFO_IF(true, "[transform] lhs of binary operator: ") <<
                misc_utils::debug_info(op) << std::endl;
            op->set_lhs_operand(SageBuilder::buildCastExp(op->get_lhs_operand(),
                get_transformation_fp(SageInterface::getGlobalScope(op))));
        }
        if (op->get_rhs_operand()->get_type()->isFloatType() &&
                op->get_rhs_operand()->get_lvalue() == false) {
            INFO_IF(true, "[transform] rhs of binary operator: ") <<
                misc_utils::debug_info(op) << std::endl;
            op->set_rhs_operand(SageBuilder::buildCastExp(op->get_rhs_operand(),
                get_transformation_fp(SageInterface::getGlobalScope(op))));
        }
    }
}
