#include "allocate_transformer.h"

AllocateTransformer::AllocateTransformer(SgProject* project) {
    m_project = project;
    m_type_trans = NULL;
    m_access_trans = NULL;
}

void AllocateTransformer::set_type_transformer(
        TypeTransformer *type_trans) {
    m_type_trans = type_trans;
}

void AllocateTransformer::set_access_transformer(
        AccessTransformer *access_trans) {
    m_access_trans = access_trans;
}

void AllocateTransformer::collect_types(void) {
    for (auto node : NodeQuery::querySubTree(m_project, V_SgExpression)) {
        auto exp = isSgExpression(node);
        SgType *type = NULL;

        if (auto cast = isSgCastExp(exp))
            type = cast->get_type();
        else if (auto assign = isSgAssignOp(exp))
            type = assign->get_lhs_operand()->get_type();
        else if (auto init = isSgInitializer(exp)) {
            if (!isSgInitializedName(init->get_parent())) continue;
            type = isSgInitializedName(init->get_parent())->get_type();
        } else if (auto func = isSgFunctionCallExp(exp)) {
            auto args = func->get_args()->get_expressions();
            if (args.empty()) continue;

            for (auto i : NodeQuery::querySubTree(args[0], V_SgCastExp)) {
                // first cast from pointer is the implied cast to void *
                auto cast = isSgCastExp(i);
                auto orig_type = cast->get_operand_i()->get_type();
                if (isSgPointerType(orig_type)) {
                    type = orig_type; break; }
            }
        }

        if (type != NULL) m_original_type[exp] = type;
    }
}

SgType *AllocateTransformer::malloc_type_from_call(SgNode *node) {
    for (auto i : NodeQuery::querySubTree(node, V_SgSizeOfOp)) {
        auto op = isSgSizeOfOp(i);
        return op->get_operand_type();
    }

    while (node != NULL) {
        node = node->get_parent();

        auto exp = isSgExpression(node);
        if (exp == NULL) continue;

        // return types collected in collect_types()
        if (!m_original_type.count(exp)) continue;
        if (auto ptr = isSgPointerType(m_original_type[exp])) {
            return ptr->get_base_type();
        }
    }

    assert(false);
    return NULL;
}

SgType *AllocateTransformer::free_type_from_call(SgNode *node) {
    assert(isSgPointerType(m_original_type[node]));
    return isSgPointerType(m_original_type[node])->get_base_type();
}

void AllocateTransformer::transform(void) {
    for (auto call : NodeQuery::querySubTree(m_project,
                V_SgFunctionCallExp)) {
        for (auto ref : NodeQuery::querySubTree(call,
                    V_SgFunctionRefExp)) {
            auto ref_v = isSgFunctionRefExp(ref);
            if (misc_utils::insideSystemHeader(ref_v)) continue;

            auto name = ref_v->get_symbol()->get_name().getString();
            SgType *type = NULL;
            SgFunctionDeclaration *func_decl = NULL;

            if (name != "malloc" && name != "free") continue;

            INFO_IF(true, "[alloc] transform ") << name << " in: " <<
                ref_v->get_file_info()->get_filenameString() << ":" <<
                ref_v->get_file_info()->get_line() << std::endl;

            if (name == "malloc") {
                type = malloc_type_from_call(call);
                INFO_IF(true, "    type is ") <<
                    type->unparseToString() << std::endl;

                func_decl = std::get<1>(m_access_trans->get_storage_of(
                            type, SageInterface::getGlobalScope(call)))
                    .malloc_func;
            } else if (name == "free") {
                type = free_type_from_call(call);
                INFO_IF(true, "    type is ") <<
                    type->unparseToString() << std::endl;

                func_decl = std::get<1>(m_access_trans->get_storage_of(
                            type, SageInterface::getGlobalScope(call)))
                    .free_func;
            }

            auto sym = isSgFunctionSymbol(func_decl->
                search_for_symbol_from_symbol_table());
            assert(sym);
            ref_v->set_symbol(sym);
        }
    }
}
