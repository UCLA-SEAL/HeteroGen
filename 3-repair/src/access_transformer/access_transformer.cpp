#include "access_transformer.h"

AccessTransformer::AccessTransformer(SgProject* project) {
    m_project = project;
    m_type_trans = NULL;
    m_is_instrument = false;
    m_list_base_decl = SageBuilder::buildClassDeclaration_nfi(
            "__dst_alloc_list_base_t", SgClassDeclaration::e_struct,
            m_project->get_globalScopeAcrossFiles(),
            NULL, NULL, NULL);
    m_list_base_decl->unsetForward();

    auto list_base_def = SageBuilder::buildClassDefinition(m_list_base_decl);
    auto list_prev = SageBuilder::buildVariableDeclaration(
            "prev", SgTypeUnsignedInt::createType(), NULL, list_base_def);
    auto list_next = SageBuilder::buildVariableDeclaration(
            "next", SgTypeUnsignedInt::createType(), NULL, list_base_def);
    list_base_def->append_member(list_prev);
    list_base_def->append_member(list_next);
}

void AccessTransformer::set_is_instrument(void) {
    m_is_instrument = true;
}

void AccessTransformer::set_type_transformer(TypeTransformer *type_trans) {
    m_type_trans = type_trans;
}

void AccessTransformer::set_type_size(std::map<std::string, int> mapping) {
    m_type_size_mapping = mapping;
}

void AccessTransformer::collect_access(void) {
    for (auto node : NodeQuery::querySubTree(m_project, V_SgExpression)) {
        auto exp = isSgExpression(node);
        auto access_ptr = get_pointer_access_type(exp);

        // is access and access type is pointer
        if (is_access(exp) && access_ptr) {
            m_original_type[exp] = access_ptr;
            DEBUG_IF(true, "[access(pre)] ") <<
                access_ptr->unparseToString() << " in " <<
                exp->get_file_info()->get_filenameString() << ":" <<
                exp->get_file_info()->get_line() << std::endl;
        }
    }
}

void AccessTransformer::transform(void) {
    SgScopeStatement *scope = NULL;

    for (auto node : NodeQuery::querySubTree(m_project, V_SgExpression)) {
        auto exp = isSgExpression(node);
        // store the last scope
        scope = SageInterface::getGlobalScope(node);

        if (!m_original_type.count(exp)) continue; // was not pointer access
        if (misc_utils::insideSystemHeader(exp)) continue;

        if (is_access(exp)) {
            auto access_type = get_pointer_access_type(exp);
            auto orig_type = m_original_type[exp];
            INFO_IF(access_type == NULL, "[access] index-based: ") <<
                "original type is " << orig_type->unparseToString() <<
                " in " << exp->get_file_info()->get_filenameString() << ":" <<
                exp->get_file_info()->get_line() << std::endl;

            // if access type is no longer pointer (index access)
            if (access_type == NULL) {
                auto base_type = orig_type->get_base_type();

                SgInitializedName *mem;
                AccessTool tool;
                std::tie(mem, tool) = get_storage_of(base_type,
                        SageInterface::getGlobalScope(exp));
                INFO_IF(true, "    access transform to: ") <<
                    mem->get_name().getString() << std::endl;

                set_pointer_access_loc(exp, mem,
                        tool.actual_access);
            }
        }
    }

    // TODO support multiple files
    SageInterface::prependStatement(m_list_base_decl, scope);
}
