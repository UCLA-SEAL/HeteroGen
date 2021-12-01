#include "transform_propagator.h"

TransformPropagator::TransformPropagator(SgProject* project) {
    m_project = project;
}

void TransformPropagator::set_addressof_propagated(
        const std::vector<std::pair<SgNode *, SgExpression *> > *edges) {
    m_addressof_edge = edges;
}

void TransformPropagator::propagate(void) {
    for (auto edge: *m_addressof_edge) {
        SgNode *to;
        SgExpression *from;
        std::tie(to, from) = edge;

        if (misc_utils::insideSystemHeader(from)) continue;

        INFO_IF(true, "[propagation] to ") <<
            misc_utils::debug_info(to) << std::endl;
        INFO_IF(true, "    from ") <<
            misc_utils::debug_info(from) << std::endl;

        // extract propagated type
        auto type = from->get_type();
        if (auto reffunc = isSgFunctionRefExp(from)) {
            auto funcdef = isSgFunctionDeclaration(
                    isSgFunctionRefExp(reffunc)->get_symbol()->
                    get_declaration()->get_definingDeclaration());

            if (!funcdef) continue; // skip undef func
            type = funcdef->get_type()->get_return_type();
        }

        // modify target type
        if (auto name = isSgInitializedName(to)) {
            name->set_type(type);
        } else if (auto func = isSgFunctionDeclaration(to)) {
            func->set_type(SageBuilder::buildFunctionType(
                    type, func->get_type()->get_argument_list()));
        }
    }

    ast_utils::transform_func_decl_matching_def(m_project);
}
