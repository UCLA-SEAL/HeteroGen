#include "access_transformer.h"

bool AccessTransformer::is_access(SgExpression *exp) {
    return (isSgArrowExp(exp) || isSgArrowStarOp(exp) ||
            isSgPointerDerefExp(exp) || isSgPntrArrRefExp(exp));
}

static SgExpression *get_access(SgBinaryOp *exp) {
    return exp->get_lhs_operand();
}

static SgExpression *get_access(SgUnaryOp *exp) {
    return exp->get_operand();
}

static SgExpression *get_access(SgExpression *exp) {
    if (auto e = isSgBinaryOp(exp)) return ::get_access(e);
    else if (auto e = isSgUnaryOp(exp)) return ::get_access(e);
    return NULL;
}

SgPointerType *AccessTransformer::get_pointer_access_type(SgExpression *exp) {
    auto access = ::get_access(exp);
    return access ? isSgPointerType(access->get_type()) : NULL;
}

static void set_pointer_access_loc(
        SgPntrArrRefExp *exp, SgExpression *exp_loc) {
    exp->set_lhs_operand(exp_loc);
    exp->set_rhs_operand(SageBuilder::buildUnsignedIntVal(0));
}

static void set_pointer_access_loc(
        SgBinaryOp *exp, SgExpression *exp_loc) {
    exp->set_lhs_operand(exp_loc);
}

static void set_pointer_access_loc(
        SgUnaryOp *exp, SgExpression *exp_loc) {
    exp->set_operand(exp_loc);
}

void AccessTransformer::set_pointer_access_loc(SgExpression *exp,
        SgInitializedName *mem, SgInitializedName *actual_loc) {

    auto scope = SageInterface::getGlobalScope(exp);

    SgExpression *offset;
    if (auto e = isSgPntrArrRefExp(exp)) {
        offset = e->get_rhs_operand();
    } else {
        offset = SageBuilder::buildUnsignedIntVal(0);
    }

    auto access = SageBuilder::buildAddressOfOp(
            SageBuilder::buildArrowExp(
                SageBuilder::buildAddOp(SageBuilder::buildAddOp(
                    SageBuilder::buildVarRefExp(mem, scope),
                    ::get_access(exp)), offset),
                SageBuilder::buildVarRefExp(actual_loc, scope)));

    if (auto e = isSgPntrArrRefExp(exp)) ::set_pointer_access_loc(e, access);
    else if (auto e = isSgBinaryOp(exp)) ::set_pointer_access_loc(e, access);
    else if (auto e = isSgUnaryOp(exp)) ::set_pointer_access_loc(e, access);
}
