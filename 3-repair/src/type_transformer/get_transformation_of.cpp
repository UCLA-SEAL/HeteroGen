#include "type_transformer.h"

SgType *TypeTransformer::get_transformation_of(
        SgPointerType *type, SgScopeStatement *scope) {
    // find equivalent type
    auto eqiv_type = type;
    for (auto mapped: m_mapping_to_index)
        if (SageInterface::isEquivalentType(mapped.first, type)) {
            eqiv_type = mapped.first;
            break;
        }

    // generate a new type
    if (!m_mapping_to_index.count(eqiv_type) && scope) {
        // create typedef
        auto name = "__didx" + type->get_base_type()->get_mangled().getString();
        auto def = SageBuilder::buildTypedefDeclaration_nfi(
                name, SgTypeUnsignedLongLong::createType(), scope);
        SageInterface::prependStatement(def, scope);
        SageInterface::attachComment(def,
                "type " + name + " was " +
                type->unparseToString());

        // set up mapping
        m_mapping_to_index[type] = def->get_type();
    }

    return m_mapping_to_index[eqiv_type];
}
