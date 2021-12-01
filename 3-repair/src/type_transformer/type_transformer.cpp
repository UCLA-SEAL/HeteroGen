#include "type_transformer.h"

TypeTransformer::TypeTransformer(SgProject* project,
        misc_utils::RefactorType t) {
    m_project = project;
    m_transform_type = NULL;
    m_type = t;
}

void TypeTransformer::set_exclusion(const std::set<SgNode *> *excluded) {
    m_excluded = excluded;
}

void TypeTransformer::set_varmap(std::map<std::string, std::string> var_name_to_type) {
    var_name_to_type_map = var_name_to_type;
}

void TypeTransformer::transform(void) {
    transform_func_return();
    transform_func_param();
    ast_utils::transform_func_decl_matching_def(m_project);

    transform_var_decl();
    transform_typecast();

    if (m_type == misc_utils::RefactorType::fp)
        transform_binary_operations();
}

SgType *TypeTransformer::get_transformation_fp(SgScopeStatement *scope) {
    if (!m_transform_type) {
        m_transform_type = SageInterface::lookupTypedefSymbolInParentScopes(
            "__fpt_t", scope)->get_type();
    }
    return m_transform_type;
}

SgType *TypeTransformer::get_transformation_i(SgScopeStatement *scope, SgName var_name) {
        if (var_name_to_type_map.find(var_name.getString()) != var_name_to_type_map.end()){
            m_transform_type = SageInterface::lookupTypedefSymbolInParentScopes(
            var_name_to_type_map[var_name.getString()], scope)->get_type();
        } else{
            m_transform_type = SageBuilder::buildIntType();
        }
    return m_transform_type;
}
