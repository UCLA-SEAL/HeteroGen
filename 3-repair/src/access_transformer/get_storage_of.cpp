#include "access_transformer.h"


static SgFunctionDeclaration *build_malloc_function(
        std::string name, SgScopeStatement *scope) {
    return SageBuilder::buildNondefiningFunctionDeclaration(
            "__dst_alloc_malloc" + name,
            SgTypeUnsignedLongLong::createType(),
            SageBuilder::buildFunctionParameterList(
                SageBuilder::buildFunctionParameterTypeList(
                    SgTypeUnsignedLongLong::createType())
            ), scope);
}

static SgFunctionDeclaration *build_free_function(
        std::string name, SgScopeStatement *scope) {
    return SageBuilder::buildNondefiningFunctionDeclaration(
            "__dst_alloc_free" + name,
            SgTypeVoid::createType(),
            SageBuilder::buildFunctionParameterList(
                SageBuilder::buildFunctionParameterTypeList(
                    SgTypeUnsignedLongLong::createType())
            ), scope);
}


std::tuple<SgInitializedName *, AccessTransformer::AccessTool>
AccessTransformer::get_storage_of(
    SgType *type, SgScopeStatement *scope) {

    // index access could only point to index access
    auto ptr = isSgPointerType(type);
    if (ptr) type = SgTypeUnsignedLongLong::createType();

    // find equivalent type
    auto eqiv_type = type;
    for (auto mapped: m_storage)
        if (SageInterface::isEquivalentType(mapped.first, type)) {
            eqiv_type = mapped.first;
            break;
        }

    if (!m_storage.count(eqiv_type)) {
        // Build storage name and freelist type name
        std::string mangled_name = type->get_mangled().getString();
        std::string name = "__dmem" + mangled_name;
        std::string freelist_name = "__dst_alloc_list" + name;

        int struct_size = 10; // default size if no invariant availible
        if (m_type_size_mapping.find(name) !=
                m_type_size_mapping.end()) {
            struct_size = m_type_size_mapping[name];
        }

        // Build freelist union structure
        auto freelist_decl = SageBuilder::buildClassDeclaration_nfi(
                freelist_name, SgClassDeclaration::e_struct, scope,
                NULL, NULL, NULL);
        freelist_decl->unsetForward();

        // Build definition
        auto freelist_def = SageBuilder::buildClassDefinition(freelist_decl);
        auto freelist_link = SageBuilder::buildVariableDeclaration(
                "_link", m_list_base_decl->get_type(), NULL, freelist_def);
        auto freelist_data = SageBuilder::buildVariableDeclaration(
                "_data", eqiv_type, NULL, freelist_def);
        freelist_def->append_member(freelist_link);
        freelist_def->append_member(freelist_data);

        auto array_type = SageBuilder::buildArrayType(freelist_decl->get_type(),
            SageBuilder::buildUnsignedIntVal((1 << struct_size) + 1));
        auto var = SageBuilder::buildInitializedName(name, array_type);
        var->set_scope(scope);
        m_storage[eqiv_type] = var;
        m_access_tool[eqiv_type].actual_access =
            freelist_data->get_decl_item("_data");

        auto mem_decl = SageBuilder::buildVariableDeclaration(
                name, array_type, NULL, scope);
        auto runtime_code = get_alloc_runtime_of(
                name, type->unparseToString(), struct_size);

        auto malloc_func = build_malloc_function(name, scope);
        auto free_func = build_free_function(name, scope);
        m_access_tool[eqiv_type].malloc_func = malloc_func;
        m_access_tool[eqiv_type].free_func = free_func;

        SgStatement *loc;
        if (auto named = isSgNamedType(type)) {
            // TODO in a header file?
            auto decl = named->get_declaration();
            if (decl->isForward() && decl->get_definingDeclaration())
                decl = decl->get_definingDeclaration();
            loc = SageInterface::getNextStatement(decl);
        } else if (auto file = isSgVariableSymbol(
                    SageInterface::lookupSymbolInParentScopes(
                    "__dst_file", scope))) {
            loc = SageInterface::getNextStatement(file
                    ->get_declaration()->get_declaration());
        } else {
            loc = SageInterface::getFirstStatement(scope);
        }

        SageInterface::insertStatementBefore(loc, freelist_decl);
        SageInterface::insertStatementBefore(loc, mem_decl);
        SageInterface::attachArbitraryText(loc, runtime_code,
                PreprocessingInfo::before);
        SageInterface::insertStatementBefore(loc, malloc_func);
        SageInterface::insertStatementBefore(loc, free_func);
    }

    return std::make_tuple(m_storage[eqiv_type], m_access_tool[eqiv_type]);
}
